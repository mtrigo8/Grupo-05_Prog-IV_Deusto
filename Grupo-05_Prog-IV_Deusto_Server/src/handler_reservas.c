#include "handler_reservas.h"
#include "protocol.h"
#include "log.h"
#include "server_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>


/* Mantenida únicamente como mecanismo de respaldo (fallback) por seguridad */
static void get_fecha_hoy(char *buffer, int size)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    if (tm_info) {
        strftime(buffer, size, "%Y-%m-%d", tm_info);
    } else {
        strncpy(buffer, "2026-01-01", size - 1);
        buffer[size - 1] = '\0';
    }
}


static int contar_reservas_servicio(sqlite3 *db, int id_servicio)
{
    sqlite3_stmt *stmt;
    int count = 0;

    const char sql[] = "SELECT COUNT(*) FROM reserva WHERE id_servicio = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;

    sqlite3_bind_int(stmt, 1, id_servicio);

    if (sqlite3_step(stmt) == SQLITE_ROW)
        count = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return count;
}


static int get_capacidad_max(sqlite3 *db, int id_servicio)
{
    sqlite3_stmt *stmt;
    int capacidad = -1;

    /* BUG FIX original: usar rowid en lugar de id_servicio si aplica */
    const char sql[] = "SELECT capacidad_max FROM servicio WHERE id_servicio = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return -1;

    sqlite3_bind_int(stmt, 1, id_servicio);

    if (sqlite3_step(stmt) == SQLITE_ROW)
        capacidad = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return capacidad;
}


static int ya_reservado(sqlite3 *db, int id_usuario, int id_servicio)
{
    sqlite3_stmt *stmt;
    int count = 0;

    const char sql[] =
        "SELECT COUNT(*) FROM reserva "
        "WHERE id_usuario = ? AND id_servicio = ?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return 0;

    sqlite3_bind_int(stmt, 1, id_usuario);
    sqlite3_bind_int(stmt, 2, id_servicio);

    if (sqlite3_step(stmt) == SQLITE_ROW)
        count = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);
    return count > 0;
}


void handler_create_reserva(SOCKET comm_socket, sqlite3 *db, char *params)
{
    char sendBuff[BUFF_SIZE];
    char msg[256];

    int id_usuario  = 0;
    int id_servicio = 0;
    char fecha_cliente[20] = "";

    // Parseo de parámetros incluyendo la fecha del cliente
    char *token = strtok(params, SEP);
    if (token) id_usuario  = atoi(token);

    token = strtok(NULL, SEP);
    if (token) id_servicio = atoi(token);

    token = strtok(NULL, SEP);
    if (token) {
        strncpy(fecha_cliente, token, sizeof(fecha_cliente) - 1);
        fecha_cliente[sizeof(fecha_cliente) - 1] = '\0';
    } else {
        // Fallback preventivo si el cliente no envía la fecha
        get_fecha_hoy(fecha_cliente, sizeof(fecha_cliente));
    }

    printf("  -> CREATE_RESERVA: id_usuario=%d id_servicio=%d fecha=%s\n",
           id_usuario, id_servicio, fecha_cliente);
    fflush(stdout);

    // Validación básica de parámetros extraídos
    if (id_usuario <= 0 || id_servicio <= 0) {
        strcpy(sendBuff, RES_ERR_GENERICO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        return;
    }

    if (ya_reservado(db, id_usuario, id_servicio))
    {
        strcpy(sendBuff, RES_ERR_YA_RESERVADO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        printf("  <- %s\n", sendBuff);
        fflush(stdout);
        return;
    }

    int capacidad = get_capacidad_max(db, id_servicio);
    int ocupadas  = contar_reservas_servicio(db, id_servicio);

    if (capacidad < 0 || ocupadas < 0 || ocupadas >= capacidad)
    {
        strcpy(sendBuff, RES_ERR_SIN_CUPOS);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        printf("  <- %s (cap=%d ocupadas=%d)\n", sendBuff, capacidad, ocupadas);
        fflush(stdout);
        return;
    }

    sqlite3_stmt *stmt;
    const char sql[] =
        "INSERT INTO reserva (id_usuario, id_servicio, fecha_reserva) "
        "VALUES (?, ?, ?)";

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        strcpy(sendBuff, RES_ERR_GENERICO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        return;
    }

    sqlite3_bind_int (stmt, 1, id_usuario);
    sqlite3_bind_int (stmt, 2, id_servicio);
    sqlite3_bind_text(stmt, 3, fecha_cliente, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    int id_reserva = (int) sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        strcpy(sendBuff, RES_ERR_GENERICO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        snprintf(msg, sizeof(msg), "Error INSERT reserva usuario=%d servicio=%d",
                 id_usuario, id_servicio);
        registrar_log(db, id_usuario, "ERROR", msg);
        server_log("ERROR", msg);
        return;
    }

    snprintf(sendBuff, sizeof(sendBuff), "%s|%d|%s",
             RES_OK, id_reserva, fecha_cliente);
    send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    printf("  <- %s\n", sendBuff);
    fflush(stdout);

    snprintf(msg, sizeof(msg), "Reserva creada: id=%d usuario=%d servicio=%d",
             id_reserva, id_usuario, id_servicio);
    registrar_log(db, id_usuario, "INFO", msg);
    server_log("INFO", msg);
}


void handler_get_reserva(SOCKET comm_socket, sqlite3 *db, char *params)
{
    char sendBuff[BUFF_SIZE];

    int id_usuario = atoi(params);
    printf("  -> GET_RESERVA: id_usuario=%d\n", id_usuario);
    fflush(stdout);

    const char sql[] =
        "SELECT r.id_reserva, r.id_servicio, s.nombre_servicio, r.fecha_reserva "
        "FROM reserva r "
        "JOIN servicio s ON r.id_servicio = s.id_servicio "
        "WHERE r.id_usuario = ?";

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        strcpy(sendBuff, RES_ERR_GENERICO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        return;
    }

    sqlite3_bind_int(stmt, 1, id_usuario);

    strcpy(sendBuff, RES_LIST_START);
    send(comm_socket, sendBuff, sizeof(sendBuff), 0);

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int         id_res  = sqlite3_column_int (stmt, 0);
        int         id_serv = sqlite3_column_int (stmt, 1);
        const char *nombre  = (const char *)sqlite3_column_text(stmt, 2);
        const char *fecha   = (const char *)sqlite3_column_text(stmt, 3);

        /* Formato esperado por el cliente: idReserva|idServicio|nombreServicio|fecha|estado */
        snprintf(sendBuff, sizeof(sendBuff), "%d|%d|%s|%s|ACTIVA",
                 id_res,
                 id_serv,
                 nombre ? nombre : "",
                 fecha  ? fecha  : "");

        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        count++;
    }

    sqlite3_finalize(stmt);

    strcpy(sendBuff, RES_LIST_END);
    send(comm_socket, sendBuff, sizeof(sendBuff), 0);

    printf("  <- LIST (%d reservas)\n", count);
    fflush(stdout);
}


void handler_cancel_reserva(SOCKET comm_socket, sqlite3 *db, char *params)
{
    char sendBuff[BUFF_SIZE];
    char msg[256];

    int id_reserva = 0;
    int id_usuario = 0;

    char *token = strtok(params, SEP);
    if (token) id_reserva = atoi(token);
    token = strtok(NULL, SEP);
    if (token) id_usuario = atoi(token);

    printf("  -> CANCEL_RESERVA: id_reserva=%d id_usuario=%d\n",
           id_reserva, id_usuario);
    fflush(stdout);

    sqlite3_stmt *stmt;
    const char sql_check[] =
        "SELECT COUNT(*) FROM reserva "
        "WHERE id_reserva = ? AND id_usuario = ?";

    int rc = sqlite3_prepare_v2(db, sql_check, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        strcpy(sendBuff, RES_ERR_GENERICO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        return;
    }

    sqlite3_bind_int(stmt, 1, id_reserva);
    sqlite3_bind_int(stmt, 2, id_usuario);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (count == 0)
    {
        strcpy(sendBuff, RES_ERR_NO_AUTORIZADO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        printf("  <- %s\n", sendBuff);
        fflush(stdout);
        return;
    }

    const char sql_del[] = "DELETE FROM reserva WHERE id_reserva = ?";

    rc = sqlite3_prepare_v2(db, sql_del, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        strcpy(sendBuff, RES_ERR_GENERICO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        return;
    }

    sqlite3_bind_int(stmt, 1, id_reserva);
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        strcpy(sendBuff, RES_ERR_NO_CANCELABLE);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        snprintf(msg, sizeof(msg), "Error DELETE reserva id=%d", id_reserva);
        registrar_log(db, id_usuario, "ERROR", msg);
        server_log("ERROR", msg);
        return;
    }

    strcpy(sendBuff, RES_CANCELADA);
    send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    printf("  <- %s\n", sendBuff);
    fflush(stdout);

    snprintf(msg, sizeof(msg), "Reserva cancelada: id=%d usuario=%d",
             id_reserva, id_usuario);
    registrar_log(db, id_usuario, "INFO", msg);
    server_log("INFO", msg);
}


void handler_update_reserva(SOCKET comm_socket, sqlite3 *db, char *params)
{
    char sendBuff[BUFF_SIZE];
    char msg[256];

    int id_reserva    = 0;
    int id_usuario    = 0;
    int id_serv_nuevo = 0;
    char fecha_cliente[20] = "";

    // Parseo de parámetros incluyendo la nueva fecha enviada por el cliente
    char *token = strtok(params, SEP);
    if (token) id_reserva    = atoi(token);
    token = strtok(NULL, SEP);
    if (token) id_usuario    = atoi(token);
    token = strtok(NULL, SEP);
    if (token) id_serv_nuevo = atoi(token);
    token = strtok(NULL, SEP);
    if (token) {
        strncpy(fecha_cliente, token, sizeof(fecha_cliente) - 1);
        fecha_cliente[sizeof(fecha_cliente) - 1] = '\0';
    } else {
        get_fecha_hoy(fecha_cliente, sizeof(fecha_cliente));
    }

    printf("  -> UPDATE_RESERVA: id_reserva=%d id_usuario=%d id_serv_nuevo=%d fecha=%s\n",
           id_reserva, id_usuario, id_serv_nuevo, fecha_cliente);
    fflush(stdout);

    // Validación elemental de tokens
    if (id_reserva <= 0 || id_usuario <= 0 || id_serv_nuevo <= 0) {
        strcpy(sendBuff, RES_ERR_GENERICO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        return;
    }

    sqlite3_stmt *stmt;
    const char sql_check[] =
        "SELECT COUNT(*) FROM reserva "
        "WHERE id_reserva = ? AND id_usuario = ?";

    // MEJORA: Añadido control de errores en el prepare de la verificación
    int rc = sqlite3_prepare_v2(db, sql_check, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        strcpy(sendBuff, RES_ERR_GENERICO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        return;
    }

    sqlite3_bind_int(stmt, 1, id_reserva);
    sqlite3_bind_int(stmt, 2, id_usuario);

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        count = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (count == 0)
    {
        strcpy(sendBuff, RES_ERR_NO_AUTORIZADO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        printf("  <- %s\n", sendBuff);
        fflush(stdout);
        return;
    }

    if (ya_reservado(db, id_usuario, id_serv_nuevo))
    {
        strcpy(sendBuff, RES_ERR_YA_RESERVADO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        printf("  <- %s\n", sendBuff);
        fflush(stdout);
        return;
    }

    int capacidad = get_capacidad_max(db, id_serv_nuevo);
    int ocupadas  = contar_reservas_servicio(db, id_serv_nuevo);

    if (capacidad < 0)
    {
        strcpy(sendBuff, RES_ERR_NO_ENCONTRADO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        printf("  <- %s\n", sendBuff);
        fflush(stdout);
        return;
    }

    if (ocupadas >= capacidad)
    {
        strcpy(sendBuff, RES_ERR_SIN_CUPOS);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        printf("  <- %s\n", sendBuff);
        fflush(stdout);
        return;
    }

    const char sql_upd[] =
        "UPDATE reserva "
        "SET id_servicio = ?, fecha_reserva = ? "
        "WHERE id_reserva = ?";

    rc = sqlite3_prepare_v2(db, sql_upd, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        strcpy(sendBuff, RES_ERR_GENERICO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        return;
    }

    sqlite3_bind_int (stmt, 1, id_serv_nuevo);
    sqlite3_bind_text(stmt, 2, fecha_cliente, -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 3, id_reserva);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE)
    {
        strcpy(sendBuff, RES_ERR_GENERICO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        snprintf(msg, sizeof(msg), "Error UPDATE reserva id=%d", id_reserva);
        registrar_log(db, id_usuario, "ERROR", msg);
        server_log("ERROR", msg);
        return;
    }

    snprintf(sendBuff, sizeof(sendBuff), "%s|%d|%s",
             RES_OK, id_reserva, fecha_cliente);
    send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    printf("  <- %s\n", sendBuff);
    fflush(stdout);

    snprintf(msg, sizeof(msg), "Reserva actualizada: id=%d nuevo_servicio=%d usuario=%d",
             id_reserva, id_serv_nuevo, id_usuario);
    registrar_log(db, id_usuario, "INFO", msg);
    server_log("INFO", msg);
}
