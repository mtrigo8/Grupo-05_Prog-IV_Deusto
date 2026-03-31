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

//Falta acabarlo
int insert_usuario(sqlite3 *db, char datos[][]) {
	sqlite3_stmt *stmt;

	char sql[] = "insert into country (id_usuario, nombre, DNI, password_hash, id_rol) values (NULL, ?, ?, ?, 2)";
	int result = sqlite3_prepare_v2(db, sql, strlen(sql) + 1, &stmt, NULL) ;
	if (result != SQLITE_OK) {
		printf("Error preparing statement (INSERT)\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

	printf("SQL query prepared (INSERT)\n");

	result = sqlite3_bind_text(stmt, 1, name, strlen(name), SQLITE_STATIC);
	if (result != SQLITE_OK) {
		printf("Error binding parameters\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

	result = sqlite3_step(stmt);
	if (result != SQLITE_DONE) {
		printf("Error inserting new data into country table\n");
		return result;
	}

	result = sqlite3_finalize(stmt);
	if (result != SQLITE_OK) {
		printf("Error finalizing statement (INSERT)\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

	printf("Prepared statement finalized (INSERT)\n");

	return SQLITE_OK;
}
