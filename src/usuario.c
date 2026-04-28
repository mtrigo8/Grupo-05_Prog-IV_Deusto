/*
 * usuario.c
 *
 *  Implementacion de las operaciones de base de datos para la
 *  estructura Usuario: registro (insert) e inicio de sesion (login).
 */

#include "../server/usuario.h"
#include "../server/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int insert_usuario(sqlite3 *db, char *datos[]) {
    sqlite3_stmt *stmt;

    if (db == NULL || datos == NULL ||
        datos[0] == NULL || datos[1] == NULL ||
        datos[2] == NULL || datos[3] == NULL) {
        printf("Error: Datos de entrada nulos.\n");
        return SQLITE_ERROR;
    }

    /* Sin id_rol: todos los usuarios en BD son consumidores */
    const char sql[] =
        "INSERT INTO usuario (id_usuario, nombre, apellido, DNI, password_hash) "
        "VALUES (NULL, ?, ?, ?, ?)";

    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error preparando el INSERT: %s\n", sqlite3_errmsg(db));
        return result;
    }

    sqlite3_bind_text(stmt, 1, datos[0], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, datos[1], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, datos[2], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, datos[3], -1, SQLITE_TRANSIENT);

    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (result != SQLITE_DONE) {
        printf("Error al insertar usuario: %s\n", sqlite3_errmsg(db));
        char msg[256];
        snprintf(msg, sizeof(msg), "Error al registrar usuario con DNI: %s", datos[2]);
        registrar_log(db, 0, "ERROR", msg);
        return result;
    }

    printf("Usuario '%s' insertado con exito.\n", datos[0]);

    char msg[256];
    snprintf(msg, sizeof(msg), "Nuevo usuario registrado: %s %s (DNI: %s)",
             datos[0], datos[1], datos[2]);
    registrar_log(db, 0, "INFO", msg);

    return SQLITE_OK;
}

Usuario login_usuario(sqlite3 *db, char *dni, char *contrasena) {
    sqlite3_stmt *stmt;
    Usuario u;
    memset(&u, 0, sizeof(Usuario));

    const char sql[] =
        "SELECT nombre, apellido, DNI, password_hash "
        "FROM usuario WHERE DNI = ? AND password_hash = ?";

    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error al preparar login: %s\n", sqlite3_errmsg(db));
        return u;
    }

    sqlite3_bind_text(stmt, 1, dni,        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, contrasena, -1, SQLITE_TRANSIENT);

    result = sqlite3_step(stmt);

    if (result == SQLITE_ROW) {
        strncpy(u.nombre,    (const char *)sqlite3_column_text(stmt, 0), sizeof(u.nombre)     - 1);
        strncpy(u.apellido,  (const char *)sqlite3_column_text(stmt, 1), sizeof(u.apellido)   - 1);
        strncpy(u.dni,       (const char *)sqlite3_column_text(stmt, 2), sizeof(u.dni)        - 1);
        strncpy(u.contrasena,(const char *)sqlite3_column_text(stmt, 3), sizeof(u.contrasena) - 1);

        sqlite3_finalize(stmt);

        char msg[256];
        snprintf(msg, sizeof(msg), "Inicio de sesion exitoso: DNI %s", dni);
        registrar_log(db, 0, "INFO", msg);
    } else {
        u.nombre[0] = '\0';
        sqlite3_finalize(stmt);

        char msg[256];
        snprintf(msg, sizeof(msg), "Intento de login fallido: DNI %s", dni);
        registrar_log(db, 0, "WARN", msg);
    }

    return u;
}
