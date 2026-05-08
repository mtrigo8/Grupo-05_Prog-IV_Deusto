/*
 * main.c (Servidor CityHub)
 *
 * IMPORTANTE: enlazar ws2_32 y sqlite3 en el proyecto.
 * En Eclipse: Project > Properties > C/C++ Build > Settings >
 * MinGW C Linker > Libraries > añadir "ws2_32" y "sqlite3"
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
#include "handler_auth.h"
#include "handler_servicios.h"
#include "handler_reservas.h"

int main(void)
{
    /* ── Cargar configuración ─────────────────────────────────────────── */
    Config cfg;
    config_cargar(&cfg);

    /* ── Inicializar log de servidor ─────────────────────────────────── */
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

    /* ── Inicializar socket ───────────────────────────────────────────── */
    SOCKET conn_socket = server_init(cfg.server_ip, cfg.server_port);
    if (conn_socket == INVALID_SOCKET) {
        sqlite3_close(db);
        server_log("ERROR", "Fallo al inicializar el socket");
        return 1;
    }

    {
        char msg[128];
        snprintf(msg, sizeof(msg), "Servidor arrancado en %s:%d", cfg.server_ip, cfg.server_port);
        server_log("INFO", msg);
    }

    /* ── Aceptar cliente ──────────────────────────────────────────────── */
    SOCKET comm_socket = server_accept(conn_socket);
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
        int bytes = recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

        if (bytes <= 0) {
            server_log("INFO", "Conexion cerrada por el cliente o error de red");
            break;
        }

        printf("Command received: %s\n", recvBuff);

        /* --- BLOQUE DE AUTENTICACIÓN --- */
        if (strcmp(recvBuff, CMD_LOGIN) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            handler_auth_login(comm_socket, db, recvBuff);
            server_log("INFO", "LOGIN procesado");
        }
        else if (strcmp(recvBuff, CMD_REGISTER) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            handler_auth_register(comm_socket, db, recvBuff);
            server_log("INFO", "REGISTER procesado");
        }
        else if (strcmp(recvBuff, CMD_LOGOUT) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            handler_auth_logout(comm_socket, db, recvBuff);
            server_log("INFO", "LOGOUT procesado");
        }

        /* --- BLOQUE DE SERVICIOS --- */
        else if (strcmp(recvBuff, CMD_GET_SERVICIOS) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            handler_servicios_get_all(comm_socket, db, recvBuff);
            server_log("INFO", "GET_SERVICIOS procesado");
        }
        else if (strcmp(recvBuff, CMD_GET_SERVICIO) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            handler_servicios_get_one(comm_socket, db, recvBuff);
            server_log("INFO", "GET_SERVICIO (individual) procesado");
        }
        else if (strcmp(recvBuff, CMD_FILTER_SERVICIOS) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            handler_servicios_filter(comm_socket, db, recvBuff);
            server_log("INFO", "FILTER_SERVICIOS procesado");
        }

        /* --- BLOQUE DE RESERVAS --- */
        else if (strcmp(recvBuff, CMD_CREATE_RESERVA) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            handler_create_reserva(comm_socket, db, recvBuff);
            server_log("INFO", "CREATE_RESERVA procesado");
        }
        else if (strcmp(recvBuff, CMD_GET_RESERVA) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            handler_get_reserva(comm_socket, db, recvBuff);
            server_log("INFO", "GET_RESERVA procesado");
        }
        else if (strcmp(recvBuff, CMD_CANCEL_RESERVA) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            handler_cancel_reserva(comm_socket, db, recvBuff);
            server_log("INFO", "CANCEL_RESERVA procesado");
        }
        else if (strcmp(recvBuff, CMD_UPDATE_RESERVA) == 0)
        {
            memset(recvBuff, 0, sizeof(recvBuff));
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            handler_update_reserva(comm_socket, db, recvBuff);
            server_log("INFO", "UPDATE_RESERVA procesado");
        }

        /* --- BLOQUE DE UTILIDADES / CONEXIÓN --- */
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
            server_log("INFO", "Cliente desconectado por peticion");
            break;
        }
        else
        {
            strncpy(sendBuff, RES_ERR_GENERICO, sizeof(sendBuff) - 1);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Unknown command: %s\n", recvBuff);
        }

    } while (1);

    /* ── Cierres ──────────────────────────────────────────────────────── */
    server_close(INVALID_SOCKET, comm_socket);
    sqlite3_close(db);
    server_log("INFO", "Servidor cerrado correctamente");

    return 0;
}
