/*
 * db.c
 *
 *  Created on: 25 mar 2026
 *      Author: markel.trigo
 */

#include <stdio.h>
#include "sqlite3.h"
#include <string.h>
#include  "db.h"


int registrar_log(sqlite3 *db, int id_usuario, const char *nivel, const char *mensaje) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO log (id_usuario, nivel, mensaje, fecha) VALUES (?, ?, ?, datetime('now'));";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return sqlite3_errcode(db);
    }

    sqlite3_bind_int(stmt, 1, id_usuario);
    sqlite3_bind_text(stmt, 2, nivel, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, mensaje, -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int borrar_reservas(sqlite3 *db) {
	sqlite3_stmt *stmt;

	char sql[] = "delete from reserva";

	int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) ;
	if (result != SQLITE_OK) {
		printf("Error preparing statement (DELETE)\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

	printf("SQL query prepared (DELETE)\n");

	result = sqlite3_step(stmt);
	if (result != SQLITE_DONE) {
		printf("Error deleting data\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

	result = sqlite3_finalize(stmt);
	if (result != SQLITE_OK) {
		printf("Error finalizing statement (DELETE)\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

	printf("Prepared statement finalized (DELETE)\n");

	return SQLITE_OK;
}

int insert_usuario(sqlite3 *db, char *datos[]) {
    sqlite3_stmt *stmt;
    // 1. Verificación de seguridad de entrada
    if (db == NULL || datos == NULL || datos[0] == NULL || datos[1] == NULL || datos[2] == NULL || datos[3] == NULL) {
        printf("Error: Datos de entrada nulos.\n");
        return SQLITE_ERROR;
    }

    char sql[] = "INSERT INTO usuario (id_usuario, nombre, apellido, DNI, password_hash, id_rol) VALUES (NULL, ?, ?, ?, 2)";

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

    if (result != SQLITE_DONE) {
        printf("Error al insertar usuario: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt); // SIEMPRE finalizar, incluso si falla
        return result;
    }

    sqlite3_finalize(stmt);
    printf("Usuario '%s' insertado con exito.\n", datos[0]);

    return SQLITE_OK;
}

Usuario login_usuario(sqlite3 *db, char *dni, char *contrasena) {
    sqlite3_stmt *stmt;
    Usuario u;

    memset(&u, 0, sizeof(Usuario));

    // SQL: Buscamos por nombre Y contraseña
    char sql[] = "SELECT nombre, apellido, DNI, password_hash FROM usuario WHERE DNI = ? AND password_hash = ?";

    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error al preparar login: %s\n", sqlite3_errmsg(db));
        return u;
    }


    sqlite3_bind_text(stmt, 1, dni, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, contrasena, -1, SQLITE_TRANSIENT);

    result = sqlite3_step(stmt);

    if (result == SQLITE_ROW) {
        strcpy(u.nombre, (char *)sqlite3_column_text(stmt, 0));
        strcpy(u.apellido, (char *)sqlite3_column_text(stmt, 1));
        strcpy(u.dni, (char *)sqlite3_column_text(stmt, 2));
        strcpy(u.contrasena, (char *)sqlite3_column_text(stmt, 3));

        printf("Login exitoso. Bienvenido %s!\n", u.nombre);
    } else {
    	u.nombre[0] = '\0';
        printf("ID de usuario o contraseña incorrectos.\n");
    }

    sqlite3_finalize(stmt);
    return u;
}
