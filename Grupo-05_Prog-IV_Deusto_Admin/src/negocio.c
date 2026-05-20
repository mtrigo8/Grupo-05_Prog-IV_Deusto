/*
 * negocio.c
 */

#include "negocio.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* ──────────────────────────────────────────────
 * Gestion de memoria
 * ────────────────────────────────────────────── */

void negocio_free(Negocio *n) {
    if (n == NULL) return;
    free(n->nombre);        n->nombre        = NULL;
    free(n->municipio);     n->municipio     = NULL;
    free(n->hora_apertura); n->hora_apertura = NULL;
    free(n->hora_cierre);   n->hora_cierre   = NULL;
    free(n->tipo);          n->tipo          = NULL;
    free(n->dias);          n->dias          = NULL;
    free(n->descripcion);   n->descripcion   = NULL;
    /* fecha y capacidad_max son int, no se liberan */
}

/* ──────────────────────────────────────────────
 * Conversion de dias
 * ────────────────────────────────────────────── */

int convertirDiasInt(char dias[]) {
    int  mascara = 0;
    char buffer[256];

    strncpy(buffer, dias, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    for (int i = 0; buffer[i] != '\0'; i++)
        buffer[i] = toupper((unsigned char)buffer[i]);

    char *token = strtok(buffer, ", ");
    while (token != NULL) {
        if      (strcmp(token, "LUNES")     == 0) mascara |= LUNES;
        else if (strcmp(token, "MARTES")    == 0) mascara |= MARTES;
        else if (strcmp(token, "MIERCOLES") == 0) mascara |= MIERCOLES;
        else if (strcmp(token, "JUEVES")    == 0) mascara |= JUEVES;
        else if (strcmp(token, "VIERNES")   == 0) mascara |= VIERNES;
        else if (strcmp(token, "SABADO")    == 0) mascara |= SABADO;
        else if (strcmp(token, "DOMINGO")   == 0) mascara |= DOMINGO;

        token = strtok(NULL, ", ");
    }

    return mascara;
}

void convertirIntDias(int mascara, char resultado[]) {
    const char *nombres_dias[] = {
        "LUNES", "MARTES", "MIERCOLES",
        "JUEVES", "VIERNES", "SABADO", "DOMINGO"
    };

    resultado[0] = '\0';
    int primero  = 1;

    for (int i = 0; i < 7; i++) {
        if (mascara & (1 << i)) {
            if (!primero) strcat(resultado, ", ");
            strcat(resultado, nombres_dias[i]);
            primero = 0;
        }
    }
}

/* ──────────────────────────────────────────────
 * Operaciones CRUD
 * ────────────────────────────────────────────── */

Negocio *get_negocios(sqlite3 *db, int *total_negocios) {
    sqlite3_stmt *stmt;

    *total_negocios = 0;

    const char sql_count[] = "SELECT COUNT(*) FROM servicio";
    if (sqlite3_prepare_v2(db, sql_count, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error preparando el COUNT: %s\n", sqlite3_errmsg(db));
        fflush(stdout);
        return NULL;
    }

    int cantidad = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        cantidad = sqlite3_column_int(stmt, 0);
    sqlite3_finalize(stmt);

    if (cantidad == 0) return NULL;

    Negocio *lista = malloc(cantidad * sizeof(Negocio));
    if (lista == NULL) {
        printf("Error: No se pudo asignar memoria para %d servicios.\n", cantidad);
        fflush(stdout);
        return NULL;
    }
    memset(lista, 0, cantidad * sizeof(Negocio));

    const char sql_datos[] =
        "SELECT nombre_servicio, municipio, hora_apertura, hora_cierre, "
        "       fecha, tipo_servicio, capacidad_max "
        "FROM servicio";

    if (sqlite3_prepare_v2(db, sql_datos, -1, &stmt, NULL) != SQLITE_OK) {
        printf("Error preparando el SELECT: %s\n", sqlite3_errmsg(db));
        fflush(stdout);
        free(lista);
        return NULL;
    }

    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && i < cantidad) {
        const char *val;

        val = (const char *)sqlite3_column_text(stmt, 0);
        lista[i].nombre        = val ? strdup(val) : NULL;

        val = (const char *)sqlite3_column_text(stmt, 1);
        lista[i].municipio     = val ? strdup(val) : NULL;

        val = (const char *)sqlite3_column_text(stmt, 2);
        lista[i].hora_apertura = val ? strdup(val) : NULL;

        val = (const char *)sqlite3_column_text(stmt, 3);
        lista[i].hora_cierre   = val ? strdup(val) : NULL;

        lista[i].fecha         = sqlite3_column_int(stmt, 4);

        val = (const char *)sqlite3_column_text(stmt, 5);
        lista[i].tipo          = val ? strdup(val) : NULL;

        /* FIX: leer capacidad_max de la BD */
        lista[i].capacidad_max = sqlite3_column_int(stmt, 6);

        i++;
    }

    sqlite3_finalize(stmt);
    *total_negocios = i;

    char msg[128];
    snprintf(msg, sizeof(msg), "Consulta de negocios: %d registros obtenidos", i);
    registrar_log(db, 0, "INFO", msg);

    return lista;
}

int insert_negocio(sqlite3 *db, Negocio n) {
    sqlite3_stmt *stmt;

    /*
     * FIX: la version anterior no incluia capacidad_max ni descripcion
     * en el INSERT, por lo que todos los negocios creados desde el Admin
     * quedaban con capacidad_max = 0 en la BD. El servidor interpretaba
     * eso como "sin cupos" y rechazaba todas las reservas con ERR|SIN_CUPOS.
     */
    const char sql[] =
        "INSERT INTO servicio "
        "(nombre_servicio, municipio, hora_apertura, hora_cierre, "
        " fecha, tipo_servicio, capacidad_max, descripcion) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error preparando el INSERT de negocio: %s\n", sqlite3_errmsg(db));
        fflush(stdout);
        return result;
    }

    sqlite3_bind_text(stmt, 1, n.nombre,        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, n.municipio,      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, n.hora_apertura,  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, n.hora_cierre,    -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 5, n.fecha);
    sqlite3_bind_text(stmt, 6, n.tipo,           -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 7, n.capacidad_max);                        /* FIX */
    sqlite3_bind_text(stmt, 8, n.descripcion,    -1, SQLITE_TRANSIENT); /* FIX */

    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    char msg[256];
    if (result != SQLITE_DONE) {
        printf("Error al insertar negocio: %s\n", sqlite3_errmsg(db));
        fflush(stdout);
        snprintf(msg, sizeof(msg), "Error al insertar negocio: %s",
                 n.nombre ? n.nombre : "(sin nombre)");
        registrar_log(db, 0, "ERROR", msg);
    } else {
        snprintf(msg, sizeof(msg),
                 "Negocio insertado: %s en %s (tipo: %s, capacidad: %d)",
                 n.nombre    ? n.nombre    : "",
                 n.municipio ? n.municipio : "",
                 n.tipo      ? n.tipo      : "",
                 n.capacidad_max);
        registrar_log(db, 0, "INFO", msg);
    }

    return result;
}

int delete_negocio(sqlite3 *db, char *nombre) {
    sqlite3_stmt *stmt;

    const char sql[] = "DELETE FROM servicio WHERE nombre_servicio = ?";

    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error preparando el DELETE de negocio: %s\n", sqlite3_errmsg(db));
        fflush(stdout);
        return result;
    }

    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_TRANSIENT);

    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    char msg[256];
    if (result != SQLITE_DONE) {
        printf("Error al borrar negocio: %s\n", sqlite3_errmsg(db));
        fflush(stdout);
        snprintf(msg, sizeof(msg), "Error al eliminar negocio: %s", nombre ? nombre : "");
        registrar_log(db, 0, "ERROR", msg);
    } else {
        snprintf(msg, sizeof(msg), "Negocio eliminado: %s", nombre ? nombre : "");
        registrar_log(db, 0, "INFO", msg);
    }

    return result;
}

int update_negocio(sqlite3 *db, char *nombre_actual, Negocio n_nuevo) {
    sqlite3_stmt *stmt;

    /*
     * FIX: la version anterior no actualizaba nombre_servicio ni capacidad_max.
     * Ahora el UPDATE incluye ambos campos:
     *   - nombre_servicio: permite renombrar el negocio desde el Admin.
     *   - capacidad_max:   permite corregir la capacidad sin borrar y reinsertar.
     */
    const char sql[] =
        "UPDATE servicio "
        "SET nombre_servicio = ?, municipio = ?, hora_apertura = ?, hora_cierre = ?, "
        "    tipo_servicio = ?, fecha = ?, capacidad_max = ? "
        "WHERE nombre_servicio = ?";

    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error preparando el UPDATE de negocio: %s\n", sqlite3_errmsg(db));
        fflush(stdout);
        return result;
    }

    sqlite3_bind_text(stmt, 1, n_nuevo.nombre,        -1, SQLITE_TRANSIENT); /* FIX: nuevo nombre */
    sqlite3_bind_text(stmt, 2, n_nuevo.municipio,     -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, n_nuevo.hora_apertura, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, n_nuevo.hora_cierre,   -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, n_nuevo.tipo,          -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 6, n_nuevo.fecha);
    sqlite3_bind_int (stmt, 7, n_nuevo.capacidad_max);                        /* FIX: capacidad */
    sqlite3_bind_text(stmt, 8, nombre_actual,         -1, SQLITE_TRANSIENT); /* WHERE */

    result = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    char msg[256];
    if (result != SQLITE_DONE) {
        printf("Error al actualizar negocio: %s\n", sqlite3_errmsg(db));
        fflush(stdout);
        snprintf(msg, sizeof(msg), "Error al actualizar negocio: %s",
                 nombre_actual ? nombre_actual : "");
        registrar_log(db, 0, "ERROR", msg);
    } else {
        snprintf(msg, sizeof(msg), "Negocio actualizado: %s -> %s en %s (capacidad: %d)",
                 nombre_actual        ? nombre_actual        : "",
                 n_nuevo.nombre       ? n_nuevo.nombre       : "",
                 n_nuevo.municipio    ? n_nuevo.municipio    : "",
                 n_nuevo.capacidad_max);
        registrar_log(db, 0, "INFO", msg);
    }

    return result;
}
