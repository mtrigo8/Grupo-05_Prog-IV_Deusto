/*
 * main.c  (Servidor CityHub)
 *
 *  Equivalente al main.c del ejemplo del profesor, pero para CityHub.
 *  Usa server_socket.h para inicializar la conexion y protocol.h
 *  para reconocer los comandos del cliente.
 *
 *  IMPORTANTE: enlazar ws2_32 y sqlite3 en el proyecto.
 *  En Eclipse: Project > Properties > C/C++ Build > Settings >
 *              MinGW C Linker > Libraries > anadir "ws2_32" y "sqlite3"
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#include "protocol.h"
#include "server_socket.h"
#include "config.h"
#include "sqlite3.h"
#include "server_log.h"

/* TODO (Bloque 3): cuando estes en ese paso, descomentar estos includes
 * y mover la logica de cada if a su handler correspondiente.
 *
 * #include "handler_auth.h"
 * #include "handler_servicios.h"
 * #include "handler_reservas.h"
 */

int main(void)
{
    /* ── Cargar configuracion ─────────────────────────────────────────── */
    Config cfg;
    config_cargar(&cfg);

    /* ── Inicializar log de servidor (ruta viene del config) ──────────── */
    server_log_init(cfg.server_log_path);
    server_log("INFO", "Cargando configuracion...");

    /* ── Abrir base de datos ──────────────────────────────────────────── */
    sqlite3 *db;
    int rc = sqlite3_open_v2(cfg.db_path, &db, SQLITE_OPEN_READWRITE, NULL);
    if (rc != SQLITE_OK) {
        printf("No se pudo abrir la base de datos: %s\n", cfg.db_path);
        server_log("ERROR", "No se pudo abrir la base de datos");
        return 1;
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    printf("Base de datos abierta: %s\n", cfg.db_path);

    /* ── Inicializar socket (IP y puerto vienen del config) ───────────── */
    SOCKET conn_socket = server_init(cfg.server_ip, cfg.server_port);
    if (conn_socket == INVALID_SOCKET) {
        sqlite3_close(db);
        server_log("ERROR", "Fallo al inicializar el socket");
        return 1;
    }

    {
        char msg[128];
        snprintf(msg, sizeof(msg), "Servidor arrancado en %s:%d",
                 cfg.server_ip, cfg.server_port);
        server_log("INFO", msg);
    }

    /* ── Aceptar cliente ──────────────────────────────────────────────── */
    SOCKET comm_socket = server_accept(conn_socket);
    /* NOTA: server_accept cierra conn_socket internamente al tener exito.
     * Si falla, conn_socket sigue abierto y hay que cerrarlo aqui. */
    if (comm_socket == INVALID_SOCKET) {
        closesocket(conn_socket);
        sqlite3_close(db);
        WSACleanup();
        server_log("ERROR", "Fallo al aceptar conexion");
        return 1;
    }
    server_log("INFO", "Cliente conectado");

    /* ── Loop principal: comando -> ejecutar -> respuesta ─────────────── */
    char sendBuff[BUFF_SIZE];
    char recvBuff[BUFF_SIZE];

    printf("Waiting for incoming commands from client...\n");
    do
    {
        memset(recvBuff, 0, sizeof(recvBuff));
        recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

        printf("Command received: %s\n", recvBuff);

        /* IMPORTANTE: usar else-if para que recvBuff no se sobreescriba
         * entre bloques. Cada if que hace recv() modifica recvBuff, por
         * lo que los if posteriores compararian datos erroneos. */

        if (strcmp(recvBuff, CMD_LOGIN) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

            char dni[65]      = {0};
            char pass_hash[65]= {0};
            char *token = strtok(recvBuff, SEP);
            if (token) strncpy(dni,       token, sizeof(dni) - 1);
            token = strtok(NULL, SEP);
            if (token) strncpy(pass_hash, token, sizeof(pass_hash) - 1);

            /* TODO: llamar a handler_auth cuando este en Bloque 3 */
            snprintf(sendBuff, sizeof(sendBuff), "OK|1|Markel|cliente");
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
            server_log("INFO", "LOGIN procesado");
        }
        else if (strcmp(recvBuff, CMD_REGISTER) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            /* TODO: handler_auth_register() */
            snprintf(sendBuff, sizeof(sendBuff), RES_OK "|1");
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
            server_log("INFO", "REGISTER procesado");
        }
        else if (strcmp(recvBuff, CMD_GET_SERVICIOS) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0); /* tipo */

            strncpy(sendBuff, RES_LIST_START, sizeof(sendBuff) - 1);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);

            strncpy(sendBuff, RES_LIST_END, sizeof(sendBuff) - 1);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: LIST\n");
            server_log("INFO", "GET_SERVICIOS procesado");
        }
        else if (strcmp(recvBuff, CMD_CREATE_RESERVA) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            /* TODO: handler_reservas_create() */
            strncpy(sendBuff, RES_ERR_SIN_CUPOS, sizeof(sendBuff) - 1);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
            server_log("WARN", "CREATE_RESERVA: sin cupos (placeholder)");
        }
        else if (strcmp(recvBuff, CMD_PING) == 0)
        {
            strncpy(sendBuff, RES_PONG, sizeof(sendBuff) - 1);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
        }
        else if (strcmp(recvBuff, CMD_DISCONNECT) == 0)
        {
            strncpy(sendBuff, RES_DESCONECTADO, sizeof(sendBuff) - 1);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
            server_log("INFO", "Cliente desconectado");
            break;
        }
        else
        {
            /* Comando desconocido */
            strncpy(sendBuff, RES_ERR_GENERICO, sizeof(sendBuff) - 1);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Unknown command: %s\n", recvBuff);
        }

    } while (1);

    /* ── Cierres ──────────────────────────────────────────────────────── */
    server_close(INVALID_SOCKET, comm_socket); /* conn_socket ya cerrado   */
    sqlite3_close(db);
    server_log("INFO", "Servidor cerrado correctamente");

    return 0;
}
