
#include "handler_conexion.h"
#include "protocol.h"
#include "log.h"
#include "server_log.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void handler_ping(SOCKET comm_socket)
{
    char sendBuff[BUFF_SIZE];
    strcpy(sendBuff, RES_PONG);
    send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    printf("  -> PING  <- PONG\n");
    fflush(stdout);
}

int handler_disconnect(SOCKET comm_socket, sqlite3 *db, char *params)
{
    char sendBuff[BUFF_SIZE];
    char msg[256];

    int id_usuario = atoi(params);
    printf("  -> DISCONNECT: id_usuario=%d\n", id_usuario);
    fflush(stdout);

    strcpy(sendBuff, RES_DESCONECTADO);
    send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    printf("  <- %s\n", sendBuff);
    fflush(stdout);

    snprintf(msg, sizeof(msg), "Cliente desconectado: id_usuario=%d", id_usuario);
    registrar_log(db, id_usuario, "INFO", msg);
    server_log("INFO", msg);

    return 0; /* indica al main que debe salir del bucle */
}
