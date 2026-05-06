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

#include "protocol.h"       /* CMD_LOGIN, CMD_GET_SERVICIOS, RES_OK... */
#include "server_socket.h"  /* server_init(), server_accept(), server_close() */
#include "config.h"         /* Config, config_cargar() */
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
    /* ── 1. Cargar configuracion (igual que en el Admin) ─────────────────── */
    Config cfg;
    config_cargar(&cfg);
    log_init(cfg.log_path);

    /* ── 2. Abrir base de datos (la misma BD que creo el Admin) ──────────── */
    sqlite3 *db;
    int rc = sqlite3_open_v2(cfg.db_path, &db, SQLITE_OPEN_READWRITE, NULL);
    if (rc != SQLITE_OK) {
        printf("No se pudo abrir la base de datos: %s\n", cfg.db_path);
        return 1;
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    printf("Base de datos abierta: %s\n", cfg.db_path);

    /* ── 3. Inicializar socket (codigo del profesor, encapsulado) ─────────── */
    SOCKET conn_socket = server_init(SERVER_IP, SERVER_PORT);
    if (conn_socket == INVALID_SOCKET) {
        sqlite3_close(db);
        return 1;
    }

    /* ── 4. Aceptar cliente (igual que en el ejemplo del profesor) ────────── */
    SOCKET comm_socket = server_accept(conn_socket);
    if (comm_socket == INVALID_SOCKET) {
        sqlite3_close(db);
        WSACleanup();
        return 1;
    }

    /* ── 5. Loop principal: recibir comando → ejecutar → responder ────────── */
    /*      Identico al do-while del ejemplo del profesor                      */
    char sendBuff[BUFF_SIZE];
    char recvBuff[BUFF_SIZE];

    printf("Waiting for incoming commands from client...\n");
    do
    {
        memset(recvBuff, 0, sizeof(recvBuff));
        recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

        printf("Command received: %s\n", recvBuff);

        /* ── LOGIN ─────────────────────────────────────────────────────────
         * Cliente envia: "LOGIN"
         * Luego envia:   "12345678A|abc123...hash"  (dni|pass_hash)
         * Servidor responde: "OK|1|Markel|cliente"  o  "ERR|CREDENCIALES_INVALIDAS"
         */
        if (strcmp(recvBuff, CMD_LOGIN) == 0)
        {
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);

            /* Separar dni y pass_hash usando strtok (igual que el profesor
             * separa los numeros de SUMAR) */
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

        /* ── REGISTER ──────────────────────────────────────────────────────
         * Cliente envia: "REGISTER"
         * Luego envia:   "Markel|Trigo|12345678A|hash..."
         * Servidor responde: "OK|1"  o  "ERR|DNI_DUPLICADO"
         */
        if (strcmp(recvBuff, CMD_REGISTER) == 0)
        {
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            /* TODO: handler_auth_register() */
            sprintf(sendBuff, RES_OK "|1");
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
        }

        /* ── GET_SERVICIOS ─────────────────────────────────────────────────
         * Cliente envia: "GET_SERVICIOS"
         * Luego envia:   "curso"  (tipo, o "todos")
         * Servidor responde: LIST_START, una fila por send(), LIST_END
         */
        if (strcmp(recvBuff, CMD_GET_SERVICIOS) == 0)
        {
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0); /* tipo */

            /* Marca de inicio de lista (igual que SUMAR-END del profesor
             * pero al reves: aqui el servidor avisa al cliente) */
            strcpy(sendBuff, RES_LIST_START);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);

            /* TODO: get_negocios(db, &total) y enviar cada fila:
             *   sprintf(sendBuff, "%d|%s|%s|...", n.id, n.nombre, n.tipo);
             *   send(comm_socket, sendBuff, sizeof(sendBuff), 0);
             */

            strcpy(sendBuff, RES_LIST_END);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: LIST\n");
        }

        /* ── CREATE_RESERVA ────────────────────────────────────────────────
         * Cliente envia: "CREATE_RESERVA"
         * Luego envia:   "1|3"  (id_usuario|id_servicio)
         * Servidor responde: "OK|5|2026-05-06"  o  "ERR|SIN_CUPOS"
         */
        if (strcmp(recvBuff, CMD_CREATE_RESERVA) == 0)
        {
            recv(comm_socket, recvBuff, sizeof(recvBuff), 0);
            /* TODO: handler_reservas_create() */
            sprintf(sendBuff, RES_ERR_SIN_CUPOS); /* placeholder */
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
        }

        /* ── PING ──────────────────────────────────────────────────────────
         * Cliente envia: "PING"  -> Servidor responde: "PONG"
         */
        if (strcmp(recvBuff, CMD_PING) == 0)
        {
            strcpy(sendBuff, RES_PONG);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
        }

        /* ── DISCONNECT ────────────────────────────────────────────────────
         * Cliente envia: "DISCONNECT"  -> Servidor cierra el bucle
         */
        if (strcmp(recvBuff, CMD_DISCONNECT) == 0)
        {
            strcpy(sendBuff, RES_DESCONECTADO);
            send(comm_socket, sendBuff, sizeof(sendBuff), 0);
            printf("Response sent: %s\n", sendBuff);
            break; /* igual que el EXIT del ejemplo del profesor */
        }

    } while (1);

    /* ── 6. Cerrar todo (igual que el final del ejemplo del profesor) ─────── */
    server_close(INVALID_SOCKET, comm_socket);
    sqlite3_close(db);

    return 0;
}
