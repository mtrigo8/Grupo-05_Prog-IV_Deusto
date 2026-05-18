
#include "request_dispatcher.h"
#include "protocol.h"
#include "handler_auth.h"
#include "handler_servicios.h"
#include "handler_reservas.h"
#include "handler_conexion.h"
#include "server_log.h"

#include <stdio.h>
#include <string.h>

int dispatch(SOCKET comm_socket, sqlite3 *db, char *mensaje)
{
    char cmd[32]         = {0};
    char params[BUFF_SIZE] = {0};

    char *sep = strchr(mensaje, '|');
    if (sep != NULL)
    {
        /* Hay parametros: copiar comando hasta el | y el resto como params */
        int len_cmd = (int)(sep - mensaje);
        if (len_cmd >= (int)sizeof(cmd)) len_cmd = sizeof(cmd) - 1;
        strncpy(cmd,    mensaje, len_cmd);
        strncpy(params, sep + 1, sizeof(params) - 1);
    }
    else
    {
        /* Sin parametros: el mensaje entero es el comando (ej: "PING") */
        strncpy(cmd, mensaje, sizeof(cmd) - 1);
    }

    printf("[DISPATCH] cmd='%s' params='%s'\n", cmd, params);
    fflush(stdout);

    /* ── Autenticacion ────────────────────────────────────────────────── */
    if (strcmp(cmd, CMD_LOGIN)    == 0) {
    	handler_auth_login    (comm_socket, db, params); return 1; }
    if (strcmp(cmd, CMD_REGISTER) == 0) {
    	handler_auth_register (comm_socket, db, params); return 1; }
    if (strcmp(cmd, CMD_LOGOUT)   == 0) {
    	handler_auth_logout   (comm_socket, db, params); return 1; }

    /* ── Servicios ────────────────────────────────────────────────────── */
    if (strcmp(cmd, CMD_GET_SERVICIOS)    == 0) {
    	handler_servicios_get_all   (comm_socket, db, params); return 1; }
    if (strcmp(cmd, CMD_GET_SERVICIO)     == 0) {
    	handler_servicios_get_one   (comm_socket, db, params); return 1; }
    if (strcmp(cmd, CMD_FILTER_SERVICIOS) == 0) {
    	handler_servicios_filter    (comm_socket, db, params); return 1; }

    /* ── Reservas ─────────────────────────────────────────────────────── */
    if (strcmp(cmd, CMD_CREATE_RESERVA) == 0) {
    	handler_create_reserva(comm_socket, db, params); return 1; }
    if (strcmp(cmd, CMD_GET_RESERVA)    == 0) {
    	handler_get_reserva   (comm_socket, db, params); return 1; }
    if (strcmp(cmd, CMD_CANCEL_RESERVA) == 0) {
    	handler_cancel_reserva(comm_socket, db, params); return 1; }
    if (strcmp(cmd, CMD_UPDATE_RESERVA) == 0) {
    	handler_update_reserva(comm_socket, db, params); return 1; }

    /* ── Conexion ─────────────────────────────────────────────────────── */
    if (strcmp(cmd, CMD_PING)       == 0) {
    	handler_ping      (comm_socket);
    	return 1; }
    if (strcmp(cmd, CMD_DISCONNECT) == 0) {
    	return handler_disconnect(comm_socket, db, params); }

    /* Comando desconocido */
    printf("[DISPATCH] Comando desconocido: '%s'\n", cmd);
    fflush(stdout);
    server_log("WARN", "Comando desconocido recibido");
    return 1;
}
