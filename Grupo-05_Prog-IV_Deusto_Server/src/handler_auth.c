#include "handler_auth.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "protocol.h"
#include "server_log.h"
#include "usuario.h"

static long long auth_get_id_usuario(sqlite3 *db, const char *dni)
{
    sqlite3_stmt *stmt = NULL;
    long long id = -1;

    const char sql[] = "SELECT id_usuario FROM usuario WHERE DNI = ? LIMIT 1";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        server_log("ERROR", "handler_auth: no se pudo preparar SELECT id_usuario");
        return -1;
    }

    sqlite3_bind_text(stmt, 1, dni, -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW)
        id = sqlite3_column_int64(stmt, 0);

    sqlite3_finalize(stmt);
    return id;
}

static void auth_send(SOCKET sock, const char *msg)
{
    char buf[BUFF_SIZE];
    memset(buf, 0, sizeof(buf));
    strncpy(buf, msg, sizeof(buf) - 1);
    send(sock, buf, sizeof(buf), 0);
}

void handler_auth_login(SOCKET comm_socket, sqlite3 *db, const char *params)
{
    //Parsear dni y password_hash
    char buf[BUFF_SIZE];
    strncpy(buf, params, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char dni[65]       = {0};
    char pass_hash[65] = {0};

    char *token = strtok(buf, SEP);
    if (token) strncpy(dni,       token, sizeof(dni) - 1);
    token = strtok(NULL, SEP);
    if (token) strncpy(pass_hash, token, sizeof(pass_hash) - 1);

    /* Validacion minima */
    if (dni[0] == '\0' || pass_hash[0] == '\0') {
        server_log("WARN", "LOGIN: parametros incompletos");
        auth_send(comm_socket, RES_ERR_CREDENCIALES);
        return;
    }

    //Validar credenciales en BD
    Usuario u = login_usuario(db, dni, pass_hash);

    if (u.nombre == NULL) {
        // Credenciales invalidas o usuario no existe
        server_log("WARN", "LOGIN: credenciales invalidas");
        auth_send(comm_socket, RES_ERR_CREDENCIALES);
        printf("Response sent: %s\n", RES_ERR_CREDENCIALES);
        fflush(stdout);
        return;
    }

    // Obtener id_usuario (login_usuario no lo devuelve)
    long long id = auth_get_id_usuario(db, dni);
    if (id < 0) {
        server_log("ERROR", "LOGIN: no se pudo recuperar id_usuario tras login");
        usuario_free(&u);
        auth_send(comm_socket, RES_ERR_GENERICO);
        return;
    }

    // Construir y enviar respuesta "OK|id|nombre|rol"

    char respuesta[BUFF_SIZE];
    snprintf(respuesta, sizeof(respuesta),
             "%s%s%lld%s%s%s%s",
             RES_OK, SEP, id, SEP, u.nombre, SEP, "cliente");

    auth_send(comm_socket, respuesta);
    printf("Response sent: %s\n", respuesta);
    fflush(stdout);

    {
        char msg[256];
        snprintf(msg, sizeof(msg),
                 "LOGIN exitoso: id=%lld nombre=%s DNI=%s",
                 id, u.nombre, dni);
        server_log("INFO", msg);
    }

    //Liberar memoria dinamica del Usuario
    usuario_free(&u);
}

void handler_auth_register(SOCKET comm_socket, sqlite3 *db, const char *params)
{
    //Parsear nombre|apellido|dni|password_hash
    char buf[BUFF_SIZE];
    strncpy(buf, params, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char nombre[128]   = {0};
    char apellido[128] = {0};
    char dni[65]       = {0};
    char pass_hash[65] = {0};

    char *token = strtok(buf, SEP);
    if (token) strncpy(nombre,    token, sizeof(nombre)    - 1);
    token = strtok(NULL, SEP);
    if (token) strncpy(apellido,  token, sizeof(apellido)  - 1);
    token = strtok(NULL, SEP);
    if (token) strncpy(dni,       token, sizeof(dni)       - 1);
    token = strtok(NULL, SEP);
    if (token) strncpy(pass_hash, token, sizeof(pass_hash) - 1);

    if (nombre[0] == '\0' || apellido[0] == '\0' ||
        dni[0]    == '\0' || pass_hash[0] == '\0') {
        server_log("WARN", "REGISTER: parametros incompletos");
        auth_send(comm_socket, RES_ERR_GENERICO);
        printf("Response sent: %s\n", RES_ERR_GENERICO);
        fflush(stdout);
        return;
    }

    //Intentar insercion
    char *datos[4] = { nombre, apellido, dni, pass_hash };
    int rc = insert_usuario(db, datos);

    //Evaluar resultado
    if (rc == SQLITE_OK) {
        long long nuevo_id = sqlite3_last_insert_rowid(db);

        char respuesta[BUFF_SIZE];
        snprintf(respuesta, sizeof(respuesta),
                 "%s%s%lld", RES_OK, SEP, nuevo_id);

        auth_send(comm_socket, respuesta);
        printf("Response sent: %s\n", respuesta);
        fflush(stdout);

        char msg[256];
        snprintf(msg, sizeof(msg),
                 "REGISTER exitoso: id=%lld nombre=%s DNI=%s",
                 nuevo_id, nombre, dni);
        server_log("INFO", msg);

    } else if (rc == SQLITE_CONSTRAINT) {

        server_log("WARN", "REGISTER: DNI ya registrado");
        auth_send(comm_socket, RES_ERR_DNI_DUP);
        printf("Response sent: %s\n", RES_ERR_DNI_DUP);
        fflush(stdout);

    } else {
        server_log("ERROR", "REGISTER: error inesperado en insert_usuario");
        auth_send(comm_socket, RES_ERR_GENERICO);
        printf("Response sent: %s\n", RES_ERR_GENERICO);
        fflush(stdout);
    }
}

void handler_auth_logout(SOCKET comm_socket, sqlite3 *db, const char *params)
{
    (void)db;

    //Parsear id_usuario
    char id_str[32] = {0};
    strncpy(id_str, params, sizeof(id_str) - 1);

    // Eliminar posibles espacios o saltos de linea
    char *end = id_str + strlen(id_str) - 1;
    while (end >= id_str && (*end == '\r' || *end == '\n' || *end == ' '))
        *end-- = '\0';

    //Log del cierre de sesion
    {
        char msg[128];
        if (id_str[0] != '\0')
            snprintf(msg, sizeof(msg), "LOGOUT: sesion cerrada para id_usuario=%s", id_str);
        else
            snprintf(msg, sizeof(msg), "LOGOUT: sesion cerrada (id desconocido)");

        server_log("INFO", msg);
    }

    //Responder al cliente
    auth_send(comm_socket, RES_SESION_CERRADA);
    printf("Response sent: %s\n", RES_SESION_CERRADA);
    fflush(stdout);
}
