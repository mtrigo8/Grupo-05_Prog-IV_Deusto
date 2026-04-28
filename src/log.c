/*
 * log.c
 *
 *  Implementacion de las utilidades de logging de CityHub:
 *    - log_init      : configura la ruta del fichero de log
 *    - registrar_log : inserta en la tabla log de la BD
 *                      y escribe en el fichero de log en disco
 */

#include "../server/log.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

static char g_log_path[256] = "";

void log_init(const char *path) {
    if (path != NULL && path[0] != '\0')
        strncpy(g_log_path, path, sizeof(g_log_path) - 1);
}

int registrar_log(sqlite3 *db, int id_usuario, const char *nivel, const char *mensaje) {
    sqlite3_stmt *stmt;
    const char *sql =
        "INSERT INTO log (id_usuario, nivel, mensaje, fecha) "
        "VALUES (?, ?, ?, datetime('now'));";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
        return sqlite3_errcode(db);

    sqlite3_bind_int (stmt, 1, id_usuario);
    sqlite3_bind_text(stmt, 2, nivel,   -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, mensaje, -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    /* Escritura adicional en fichero de log */
    if (g_log_path[0] != '\0') {
        FILE *f = fopen(g_log_path, "a");
        if (f != NULL) {
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            char fecha[20];
            strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", tm_info);
            fprintf(f, "[%s] [%-5s] usuario_id=%d | %s\n",
                    fecha, nivel, id_usuario, mensaje);
            fclose(f);
        }
    }

    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}
