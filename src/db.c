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
