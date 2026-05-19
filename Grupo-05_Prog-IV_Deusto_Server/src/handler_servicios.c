#include "handler_servicios.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "negocio.h"
#include "protocol.h"
#include "server_log.h"

static void svc_send(SOCKET sock, const char *msg)
{
    char buf[BUFF_SIZE];
    memset(buf, 0, sizeof(buf));
    strncpy(buf, msg, sizeof(buf) - 1);
    send(sock, buf, sizeof(buf), 0);
}

static void svc_format_negocio_con_id(long long id, const Negocio *n,
                                      int capacidad,
                                      const char *descripcion,
                                      char *out, size_t out_size)
{
    char dias_str[128] = {0};
    convertirIntDias(n->fecha, dias_str);

    const char *nombre        = "";
    const char *municipio     = "";
    const char *hora_apertura = "";
    const char *hora_cierre   = "";
    const char *tipo          = "";

    if (n->nombre        != NULL) { nombre        = n->nombre;        }
    if (n->municipio     != NULL) { municipio     = n->municipio;     }
    if (n->hora_apertura != NULL) { hora_apertura = n->hora_apertura; }
    if (n->hora_cierre   != NULL) { hora_cierre   = n->hora_cierre;   }
    if (n->tipo          != NULL) { tipo          = n->tipo;          }
    if (descripcion      == NULL) { descripcion   = "";               }

    /* Protocolo: id|nombre|municipio|hora_apertura|hora_cierre|dias|tipo|capacidad|descripcion */
    snprintf(out, out_size, "%lld%s%s%s%s%s%s%s%s%s%s%s%s%s%d%s%s",
             id,            SEP,
             nombre,        SEP,
             municipio,     SEP,
             hora_apertura, SEP,
             hora_cierre,   SEP,
             dias_str,      SEP,
             tipo,          SEP,
             capacidad,     SEP,
             descripcion);
}

static long long svc_fill_negocio_from_stmt(sqlite3_stmt *stmt, Negocio *n)
{
    long long rowid = sqlite3_column_int64(stmt, 0);
    const char *val = NULL;

    val = (const char *)sqlite3_column_text(stmt, 1);
    if (val != NULL) { n->nombre        = strdup(val); }
    else             { n->nombre        = NULL;        }

    val = (const char *)sqlite3_column_text(stmt, 2);
    if (val != NULL) { n->municipio     = strdup(val); }
    else             { n->municipio     = NULL;        }

    val = (const char *)sqlite3_column_text(stmt, 3);
    if (val != NULL) { n->hora_apertura = strdup(val); }
    else             { n->hora_apertura = NULL;        }

    val = (const char *)sqlite3_column_text(stmt, 4);
    if (val != NULL) { n->hora_cierre   = strdup(val); }
    else             { n->hora_cierre   = NULL;        }

    n->fecha = sqlite3_column_int(stmt, 5);

    val = (const char *)sqlite3_column_text(stmt, 6);
    if (val != NULL) { n->tipo          = strdup(val); }
    else             { n->tipo          = NULL;        }

    return rowid;
}

/*
 * BUG FIX: la implementacion original llamaba a get_negocios() que hace
 * "SELECT nombre_servicio, municipio, ..." SIN el rowid, y luego formateaba
 * cada negocio con svc_format_negocio() (sin id).
 *
 * El cliente necesita el id del servicio para poder crear reservas.
 * Sin el id el cliente guardaba idServicio=0 en todos los negocios
 * y las reservas fallaban con ERR|SIN_CUPOS porque el servicio con
 * id=0 no existe en la BD.
 *
 * Fix: hacer la query directamente con rowid, igual que
 * handler_servicios_filter, y formatear con svc_format_negocio_con_id.
 */
void handler_servicios_get_all(SOCKET comm_socket, sqlite3 *db,
                               const char *params)
{
    char tipo_filtro[64] = {0};
    strncpy(tipo_filtro, params, sizeof(tipo_filtro) - 1);

    char *end = tipo_filtro + strlen(tipo_filtro) - 1;
    while (end >= tipo_filtro && (*end == '\r' || *end == '\n' || *end == ' '))
    {
        *end = '\0';
        end--;
    }

    int filtrar_tipo = 0;
    if (tipo_filtro[0] != '\0' && strcmp(tipo_filtro, "TODOS") != 0)
    {
        filtrar_tipo = 1;
    }

    /* Query con rowid para que el cliente pueda identificar cada servicio */
    char sql[512];
    if (filtrar_tipo)
    {
        snprintf(sql, sizeof(sql),
            "SELECT id_servicio, nombre_servicio, municipio, hora_apertura, hora_cierre, "
            "       fecha, tipo_servicio, capacidad_max, descripcion "
            "FROM servicio "
            "WHERE tipo_servicio = ?");
    }
    else
    {
        snprintf(sql, sizeof(sql),
            "SELECT id_servicio, nombre_servicio, municipio, hora_apertura, hora_cierre, "
            "       fecha, tipo_servicio, capacidad_max, descripcion "
            "FROM servicio");
    }

    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        server_log("ERROR", "GET_SERVICIOS: fallo al preparar SELECT");
        svc_send(comm_socket, RES_ERR_GENERICO);
        printf("Response sent: %s\n", RES_ERR_GENERICO);
        fflush(stdout);
        return;
    }

    if (filtrar_tipo)
    {
        sqlite3_bind_text(stmt, 1, tipo_filtro, -1, SQLITE_TRANSIENT);
    }

    svc_send(comm_socket, RES_LIST_START);

    int enviados = 0;
    char linea[BUFF_SIZE];

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Negocio n;
        memset(&n, 0, sizeof(Negocio));

        long long rowid = svc_fill_negocio_from_stmt(stmt, &n);

        int         capacidad   = sqlite3_column_int (stmt, 7);
        const char *descripcion = (const char *)sqlite3_column_text(stmt, 8);

        svc_format_negocio_con_id(rowid, &n, capacidad, descripcion,
                                  linea, sizeof(linea));
        svc_send(comm_socket, linea);
        enviados++;

        negocio_free(&n);
    }

    sqlite3_finalize(stmt);
    svc_send(comm_socket, RES_LIST_END);

    char msg[128];
    if (filtrar_tipo)
    {
        snprintf(msg, sizeof(msg),
                 "GET_SERVICIOS: %d servicios enviados (tipo=%s)",
                 enviados, tipo_filtro);
    }
    else
    {
        snprintf(msg, sizeof(msg),
                 "GET_SERVICIOS: %d servicios enviados (todos los tipos)",
                 enviados);
    }
    server_log("INFO", msg);
    printf("Response sent: LIST (%d elementos)\n", enviados);
    fflush(stdout);
}

void handler_servicios_get_one(SOCKET comm_socket, sqlite3 *db,
                               const char *params)
{
    char id_str[32] = {0};
    strncpy(id_str, params, sizeof(id_str) - 1);

    char *end = id_str + strlen(id_str) - 1;
    while (end >= id_str && (*end == '\r' || *end == '\n' || *end == ' '))
    {
        *end = '\0';
        end--;
    }

    long long id = atoll(id_str);
    if (id_str[0] == '\0' || id <= 0)
    {
        server_log("WARN", "GET_SERVICIO: id invalido");
        svc_send(comm_socket, RES_ERR_NO_ENCONTRADO);
        printf("Response sent: %s\n", RES_ERR_NO_ENCONTRADO);
        fflush(stdout);
        return;
    }

    sqlite3_stmt *stmt = NULL;
    const char sql[] =
        "SELECT id_servicio, nombre_servicio, municipio, hora_apertura, hora_cierre, "
        "       fecha, tipo_servicio, capacidad_max, descripcion "
        "FROM servicio "
        "WHERE id_servicio = ? "
        "LIMIT 1";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        server_log("ERROR", "GET_SERVICIO: fallo al preparar SELECT por rowid");
        svc_send(comm_socket, RES_ERR_GENERICO);
        printf("Response sent: %s\n", RES_ERR_GENERICO);
        fflush(stdout);
        return;
    }

    sqlite3_bind_int64(stmt, 1, id);

    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        sqlite3_finalize(stmt);
        server_log("WARN", "GET_SERVICIO: id no encontrado");
        svc_send(comm_socket, RES_ERR_NO_ENCONTRADO);
        printf("Response sent: %s\n", RES_ERR_NO_ENCONTRADO);
        fflush(stdout);
        return;
    }

    Negocio n;
    memset(&n, 0, sizeof(Negocio));
    long long rowid     = svc_fill_negocio_from_stmt(stmt, &n);
    int       capacidad = sqlite3_column_int (stmt, 7);
    const char *descripcion = (const char *)sqlite3_column_text(stmt, 8);
    sqlite3_finalize(stmt);

    char linea[BUFF_SIZE];
    svc_send(comm_socket, RES_LIST_START);
    svc_format_negocio_con_id(rowid, &n, capacidad, descripcion,
                              linea, sizeof(linea));
    svc_send(comm_socket, linea);
    svc_send(comm_socket, RES_LIST_END);

    char msg[128];
    if (n.nombre != NULL)
    {
        snprintf(msg, sizeof(msg),
                 "GET_SERVICIO: enviado id=%lld nombre=%s", rowid, n.nombre);
    }
    else
    {
        snprintf(msg, sizeof(msg),
                 "GET_SERVICIO: enviado id=%lld nombre=(null)", rowid);
    }
    server_log("INFO", msg);
    printf("Response sent: LIST (1 elemento, id=%lld)\n", rowid);
    fflush(stdout);

    negocio_free(&n);
}

void handler_servicios_filter(SOCKET comm_socket, sqlite3 *db,
                              const char *params)
{
    char buf[BUFF_SIZE];
    strncpy(buf, params, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char tipo_filtro[64] = {0};
    int  fecha_mascara   = 0;

    char *token = strtok(buf, SEP);
    if (token != NULL) { strncpy(tipo_filtro, token, sizeof(tipo_filtro) - 1); }
    token = strtok(NULL, SEP);
    if (token != NULL) { fecha_mascara = atoi(token); }

    int filtrar_tipo  = 0;
    int filtrar_fecha = 0;

    if (tipo_filtro[0] != '\0' && strcmp(tipo_filtro, "TODOS") != 0)
    {
        filtrar_tipo = 1;
    }
    if (fecha_mascara != 0)
    {
        filtrar_fecha = 1;
    }

    char sql[512];
    int  offset = 0;

    offset += snprintf(sql + offset, sizeof(sql) - offset,
        "SELECT id_servicio, nombre_servicio, municipio, hora_apertura, hora_cierre, "
        "       fecha, tipo_servicio, capacidad_max, descripcion "
        "FROM servicio "
        "WHERE 1=1 ");

    if (filtrar_tipo)
    {
        offset += snprintf(sql + offset, sizeof(sql) - offset,
                           "AND tipo_servicio = ? ");
    }
    if (filtrar_fecha)
    {
        offset += snprintf(sql + offset, sizeof(sql) - offset,
                           "AND (fecha & ?) != 0 ");
    }

    sqlite3_stmt *stmt = NULL;
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK)
    {
        server_log("ERROR", "FILTER_SERVICIOS: fallo al preparar SELECT");
        svc_send(comm_socket, RES_ERR_GENERICO);
        printf("Response sent: %s\n", RES_ERR_GENERICO);
        fflush(stdout);
        return;
    }

    int bind_idx = 1;
    if (filtrar_tipo)
    {
        sqlite3_bind_text(stmt, bind_idx, tipo_filtro, -1, SQLITE_TRANSIENT);
        bind_idx++;
    }
    if (filtrar_fecha)
    {
        sqlite3_bind_int(stmt, bind_idx, fecha_mascara);
        bind_idx++;
    }

    svc_send(comm_socket, RES_LIST_START);

    int enviados = 0;
    char linea[BUFF_SIZE];

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        Negocio n;
        memset(&n, 0, sizeof(Negocio));

        long long rowid     = svc_fill_negocio_from_stmt(stmt, &n);
        int       capacidad = sqlite3_column_int (stmt, 7);
        const char *descripcion = (const char *)sqlite3_column_text(stmt, 8);

        svc_format_negocio_con_id(rowid, &n, capacidad, descripcion,
                                  linea, sizeof(linea));
        svc_send(comm_socket, linea);
        enviados++;

        negocio_free(&n);
    }

    sqlite3_finalize(stmt);
    svc_send(comm_socket, RES_LIST_END);

    char dias_str[128] = {0};
    if (filtrar_fecha)
    {
        convertirIntDias(fecha_mascara, dias_str);
    }
    else
    {
        strncpy(dias_str, "TODOS", sizeof(dias_str) - 1);
    }

    char msg[256];
    if (filtrar_tipo)
    {
        snprintf(msg, sizeof(msg),
                 "FILTER_SERVICIOS: %d resultados (tipo=%s, dias=%s)",
                 enviados, tipo_filtro, dias_str);
    }
    else
    {
        snprintf(msg, sizeof(msg),
                 "FILTER_SERVICIOS: %d resultados (tipo=TODOS, dias=%s)",
                 enviados, dias_str);
    }
    server_log("INFO", msg);
    printf("Response sent: LIST (%d elementos)\n", enviados);
    fflush(stdout);
}
