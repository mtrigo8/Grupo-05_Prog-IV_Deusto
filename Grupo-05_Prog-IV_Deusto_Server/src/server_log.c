/*
 * server_log.c
 *
 *  Implementacion del log del servidor CityHub.
 *  Mismo estilo que log.c del Admin, pero solo escribe en fichero
 *  (sin BD) porque registra eventos de red, no acciones de usuario.
 *
 *  Formato de cada linea:
 *    [2026-05-07 12:34:56] [INFO ] Cliente conectado: 127.0.0.1
 *    [2026-05-07 12:34:57] [ERROR] recv() fallido
 */

#include "server_log.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

/* Ruta del fichero de log (se fija con server_log_init) */
static char g_log_path[256] = "";

/* ─────────────────────────────────────────────────────────────────────────
 * server_log_init
 * Igual que log_init() del Admin: guarda la ruta en una variable global.
 * ───────────────────────────────────────────────────────────────────────── */
void server_log_init(const char *path)
{
    if (path != NULL && path[0] != '\0')
        strncpy(g_log_path, path, sizeof(g_log_path) - 1);
}

/* ─────────────────────────────────────────────────────────────────────────
 * server_log
 * Escribe una linea en el fichero de log y tambien por pantalla.
 * ───────────────────────────────────────────────────────────────────────── */
void server_log(const char *nivel, const char *mensaje)
{
    /* Obtener timestamp (mismo formato que log.c del Admin) */
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char fecha[20];
    strftime(fecha, sizeof(fecha), "%Y-%m-%d %H:%M:%S", tm_info);

    /* Imprimir siempre por pantalla */
    printf("[%s] [%-5s] %s\n", fecha, nivel, mensaje);

    /* Si hay ruta configurada, escribir tambien en el fichero */
    if (g_log_path[0] != '\0') {
        FILE *f = fopen(g_log_path, "a");
        if (f != NULL) {
            fprintf(f, "[%s] [%-5s] %s\n", fecha, nivel, mensaje);
            fclose(f);
        }
    }
}
