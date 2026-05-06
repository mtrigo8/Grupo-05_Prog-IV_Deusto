/*
 * server_socket.c
 *
 *  Implementacion de las funciones de socket del servidor CityHub.
 *  Basado directamente en el ejemplo del profesor (SSC_04_Ejemplo_protocolo_CS).
 *
 *  IMPORTANTE: enlazar la libreria Winsock ("ws2_32") en el proyecto.
 */

#include "server_socket.h"

#include <stdio.h>
#include <winsock2.h>

/* ─────────────────────────────────────────────────────────────────────────
 * server_init
 * Equivalente al bloque de inicializacion del main.c del profesor,
 * pero extraido a una funcion para poder reutilizarlo.
 * ───────────────────────────────────────────────────────────────────────── */
SOCKET server_init(const char *ip, int port)
{
    WSADATA wsaData;
    SOCKET conn_socket;
    struct sockaddr_in server;

    /* Inicializar Winsock */
    printf("\nInitialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }
    printf("Initialised.\n");

    /* Crear socket */
    if ((conn_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        WSACleanup();
        return INVALID_SOCKET;
    }
    printf("Socket created.\n");

    /* Configurar direccion y puerto */
    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family      = AF_INET;
    server.sin_port        = htons(port);

    /* BIND (asociar IP/puerto al socket) */
    if (bind(conn_socket, (struct sockaddr *) &server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code: %d\n", WSAGetLastError());
        closesocket(conn_socket);
        WSACleanup();
        return INVALID_SOCKET;
    }
    printf("Bind done.\n");

    /* LISTEN (poner el socket en modo escucha, maximo 1 cliente en cola) */
    if (listen(conn_socket, 1) == SOCKET_ERROR) {
        printf("Listen failed with error code: %d\n", WSAGetLastError());
        closesocket(conn_socket);
        WSACleanup();
        return INVALID_SOCKET;
    }

    return conn_socket;
}

/* ─────────────────────────────────────────────────────────────────────────
 * server_accept
 * Bloquea hasta que un cliente se conecta y devuelve el socket
 * de comunicacion con ese cliente (comm_socket en el ejemplo del profesor).
 * ───────────────────────────────────────────────────────────────────────── */
SOCKET server_accept(SOCKET conn_socket)
{
    SOCKET comm_socket;
    struct sockaddr_in client;
    int stsize = sizeof(struct sockaddr);

    /* ACCEPT (esperar conexion entrante) */
    printf("Waiting for incoming connections...\n");
    comm_socket = accept(conn_socket, (struct sockaddr *) &client, &stsize);

    if (comm_socket == INVALID_SOCKET) {
        printf("accept failed with error code: %d\n", WSAGetLastError());
        return INVALID_SOCKET;
    }

    printf("Incoming connection from: %s (%d)\n",
           inet_ntoa(client.sin_addr),
           ntohs(client.sin_port));

    /* Cerrar el socket de escucha: ya no se necesita.
     * A partir de aqui toda la comunicacion va por comm_socket. */
    closesocket(conn_socket);

    return comm_socket;
}

/* ─────────────────────────────────────────────────────────────────────────
 * server_close
 * Cierra sockets y limpia Winsock. Llamar siempre al terminar.
 * ───────────────────────────────────────────────────────────────────────── */
void server_close(SOCKET conn_socket, SOCKET comm_socket)
{
    if (comm_socket  != INVALID_SOCKET) closesocket(comm_socket);
    if (conn_socket  != INVALID_SOCKET) closesocket(conn_socket);
    WSACleanup();
    printf("Server closed.\n");
}
