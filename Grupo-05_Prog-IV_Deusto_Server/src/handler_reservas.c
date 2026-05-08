/*
 * handler_reservas.c
 *
 *  Implementacion de los handlers de reservas del servidor CityHub.
 *  Mismo estilo que negocio.c del Admin:
 *    - sqlite3_prepare_v2 / bind / step / finalize
 *    - registrar_log para cada operacion
 *    - send() para responder al cliente siguiendo protocol.h
 */

#include "handler_reservas.h"
#include "protocol.h"
#include "log.h"
#include "server_log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>


static void get_fecha_hoy(char *buffer, int size)
{
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d", tm_info);
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


static int get_capacidad_maxima(sqlite3 *db, int id_servicio)
{
    sqlite3_stmt *stmt;
    int capacidad = -1;

    const char sql[] = "SELECT capacidad_maxima FROM servicio WHERE id_servicio = ?";

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

    /* Separar id_usuario e id_servicio del params "1|3" */
    int id_usuario  = 0;
    int id_servicio = 0;

    char *token = strtok(params, SEP);
    if (token) id_usuario  = atoi(token);
    token = strtok(NULL, SEP);
    if (token) id_servicio = atoi(token);

    printf("  -> CREATE_RESERVA: id_usuario=%d id_servicio=%d\n",
           id_usuario, id_servicio);


    if (ya_reservado(db, id_usuario, id_servicio))
    {
        strcpy(sendBuff, RES_ERR_YA_RESERVADO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        printf("  <- %s\n", sendBuff);
        return;
    }

    /* Comprobar aforo: reservas actuales < capacidad_maxima */
    int capacidad = get_capacidad_maxima(db, id_servicio);
    int ocupadas  = contar_reservas_servicio(db, id_servicio);

    if (capacidad < 0 || ocupadas < 0 || ocupadas >= capacidad)
    {
        strcpy(sendBuff, RES_ERR_SIN_CUPOS);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        printf("  <- %s (cap=%d ocupadas=%d)\n", sendBuff, capacidad, ocupadas);
        return;
    }

    //Insertar reserva
    sqlite3_stmt *stmt;
    char fecha_hoy[20];
    get_fecha_hoy(fecha_hoy, sizeof(fecha_hoy));

    const char sql[] =
        "INSERT INTO reserva (id_usuario, id_servicio, fecha_registro) "
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
    sqlite3_bind_text(stmt, 3, fecha_hoy, -1, SQLITE_TRANSIENT);

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
             RES_OK, id_reserva, fecha_hoy);
    send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    printf("  <- %s\n", sendBuff);

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

    const char sql[] =
        "SELECT r.id_reserva, s.nombre_servicio, r.fecha_registro "
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
        const char *nombre  = (const char *)sqlite3_column_text(stmt, 1);
        const char *fecha   = (const char *)sqlite3_column_text(stmt, 2);

        snprintf(sendBuff, sizeof(sendBuff), "%d|%s|%s",
                 id_res,
                 nombre ? nombre : "",
                 fecha  ? fecha  : "");

        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        count++;
    }

    sqlite3_finalize(stmt);

    strcpy(sendBuff, RES_LIST_END);
    send(comm_socket, sendBuff, sizeof(sendBuff), 0);

    printf("  <- LIST (%d reservas)\n", count);
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

    /* Comprobar que la reserva existe Y pertenece a ese usuario
     * (mismo patron que delete_negocio, pero con comprobacion previa) */
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
        /* No existe o no es del usuario */
        strcpy(sendBuff, RES_ERR_NO_AUTORIZADO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        printf("  <- %s\n", sendBuff);
        return;
    }

    /* Eliminar (mismo patron que delete_negocio) */
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

    snprintf(msg, sizeof(msg), "Reserva cancelada: id=%d usuario=%d",
             id_reserva, id_usuario);
    registrar_log(db, id_usuario, "INFO", msg);
    server_log("INFO", msg);
}


void handler_update_reserva(SOCKET comm_socket, sqlite3 *db, char *params)
{
    char sendBuff[BUFF_SIZE];
    char msg[256];

    /* Separar id_reserva, id_usuario e id_servicio_nuevo del params "5|1|7" */
    int id_reserva      = 0;
    int id_usuario      = 0;
    int id_serv_nuevo   = 0;

    char *token = strtok(params, SEP);
    if (token) id_reserva    = atoi(token);
    token = strtok(NULL, SEP);
    if (token) id_usuario    = atoi(token);
    token = strtok(NULL, SEP);
    if (token) id_serv_nuevo = atoi(token);

    printf("  -> UPDATE_RESERVA: id_reserva=%d id_usuario=%d id_serv_nuevo=%d\n",
           id_reserva, id_usuario, id_serv_nuevo);

    /* Comprobar que la reserva pertenece al usuario */
    sqlite3_stmt *stmt;
    const char sql_check[] =
        "SELECT COUNT(*) FROM reserva "
        "WHERE id_reserva = ? AND id_usuario = ?";

    sqlite3_prepare_v2(db, sql_check, -1, &stmt, NULL);
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
        return;
    }

    /* Comprobar aforo del nuevo servicio */
    int capacidad = get_capacidad_maxima(db, id_serv_nuevo);
    int ocupadas  = contar_reservas_servicio(db, id_serv_nuevo);

    if (capacidad < 0 || ocupadas >= capacidad)
    {
        strcpy(sendBuff, RES_ERR_SIN_CUPOS);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        printf("  <- %s\n", sendBuff);
        return;
    }

    /* Actualizar (mismo patron que update_negocio) */
    char fecha_hoy[20];
    get_fecha_hoy(fecha_hoy, sizeof(fecha_hoy));

    const char sql_upd[] =
        "UPDATE reserva "
        "SET id_servicio = ?, fecha_registro = ? "
        "WHERE id_reserva = ?";

    int rc = sqlite3_prepare_v2(db, sql_upd, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        strcpy(sendBuff, RES_ERR_GENERICO);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
        return;
    }

    sqlite3_bind_int (stmt, 1, id_serv_nuevo);
    sqlite3_bind_text(stmt, 2, fecha_hoy, -1, SQLITE_TRANSIENT);
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

    /* Respuesta de exito: "OK|id_reserva|nueva_fecha" */
    snprintf(sendBuff, sizeof(sendBuff), "%s|%d|%s",
             RES_OK, id_reserva, fecha_hoy);
    send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    printf("  <- %s\n", sendBuff);

    snprintf(msg, sizeof(msg), "Reserva actualizada: id=%d nuevo_servicio=%d usuario=%d",
             id_reserva, id_serv_nuevo, id_usuario);
    registrar_log(db, id_usuario, "INFO", msg);
    server_log("INFO", msg);
}
