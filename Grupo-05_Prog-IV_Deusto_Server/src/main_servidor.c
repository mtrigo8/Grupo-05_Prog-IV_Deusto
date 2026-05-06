/*
 * main.c  (Servidor CityHub)
 *
 *  Equivalente al main.c del ejemplo del profesor, pero para CityHub.
 *  Usa server_socket.h para inicializar la conexion y protocol.h
 *  para reconocer los comandos del cliente.
 *
 *  IMPORTANTE: enlazar ws2_32 y sqlite3 en el proyecto.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include "protocol.h"
#include "server_socket.h"
#include "config.h"
#include "sqlite3.h"
#include "log.h"

/* TODO (Bloque 3): cuando estes en ese paso, descomentar estos includes
 * y mover la logica de cada if a su handler correspondiente.
 *
 * #include "handler_auth.h"
 * #include "handler_servicios.h"
 * #include "handler_reservas.h"
 */

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT  6000

int main(void)
{
    Config cfg;
    config_cargar(&cfg);
    log_init(cfg.log_path);

    sqlite3 *db;
    int rc = sqlite3_open_v2(cfg.db_path, &db, SQLITE_OPEN_READWRITE, NULL);
    if (rc != SQLITE_OK) {
        printf("No se pudo abrir la base de datos: %s\n", cfg.db_path);
        return 1;
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    printf("Base de datos abierta: %s\n", cfg.db_path);

    //Inicializar socket
    SOCKET conn_socket = server_init(SERVER_IP, SERVER_PORT);
    if (conn_socket == INVALID_SOCKET) {
        sqlite3_close(db);
        return 1;
    }

    //Aceptar cliente
    SOCKET comm_socket = server_accept(conn_socket);
    if (comm_socket == INVALID_SOCKET) {
        sqlite3_close(db);
        WSACleanup();
        return 1;
    }
    //Loop de comando -> ejecutar -> respuesta
    char sendBuff[BUFF_SIZE];
    char recvBuff[BUFF_SIZE];

    printf("Waiting for incoming commands from client...\n");
    do
    {
        memset(recvBuff, 0, sizeof(recvBuff));
        recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

        printf("Command received: %s\n", recvBuff);


        if (strcmp(recvBuff, CMD_LOGIN) == 0)
        {
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

            //Separar valores de inicio de sesion
            char dni[65], pass_hash[65];
            char *token = strtok(recvBuff, SEP);
            if (token) strncpy(dni,       token, sizeof(dni) - 1);
            token = strtok(NULL, SEP);
            if (token) strncpy(pass_hash, token, sizeof(pass_hash) - 1);

            /* TODO: llamar a handler_auth cuando este en Bloque 3
             * Por ahora: respuesta de prueba */
            sprintf(sendBuff, "OK|1|Markel|cliente");
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
        }
        if (strcmp(recvBuff, CMD_REGISTER) == 0)
        {
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            /* TODO: handler_auth_register() */
            sprintf(sendBuff, RES_OK "|1");
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
        }
        if (strcmp(recvBuff, CMD_GET_SERVICIOS) == 0)
        {
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0); /* tipo */

            strcpy(sendBuff, RES_LIST_START);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);

            strcpy(sendBuff, RES_LIST_END);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: LIST\n");
        }

        if (strcmp(recvBuff, CMD_CREATE_RESERVA) == 0)
        {
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            /* TODO: handler_reservas_create() */
            sprintf(sendBuff, RES_ERR_SIN_CUPOS); /* placeholder */
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
        }
        //Ping pong
        if (strcmp(recvBuff, CMD_PING) == 0)
        {
            strcpy(sendBuff, RES_PONG);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
        }

        //Disconect
        if (strcmp(recvBuff, CMD_DISCONNECT) == 0)
        {
            strcpy(sendBuff, RES_DESCONECTADO);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
            break; /* igual que el EXIT del ejemplo del profesor */
        }

    } while (1);

    //Cierres
    server_close(INVALID_SOCKET, comm_socket);
    sqlite3_close(db);

    return 0;
}
