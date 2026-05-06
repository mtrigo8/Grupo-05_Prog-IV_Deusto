/*
 * server_socket.h
 *
 *  Funciones para inicializar el socket del servidor CityHub.
 *  Basado en el ejemplo del profesor (SSC_04_Ejemplo_protocolo_CS).
 */

#ifndef SERVER_SOCKET_H_
#define SERVER_SOCKET_H_

#include <winsock2.h>

/*
 * Inicializa Winsock, crea el socket, hace bind y listen.
 * Devuelve el socket de escucha listo para llamar a accept(),
 * o INVALID_SOCKET si algo falla.
 *
 * Uso:
 *   SOCKET conn_socket = server_init("127.0.0.1", 6000);
 *   if (conn_socket == INVALID_SOCKET) { return -1; }
 */
SOCKET server_init(const char *ip, int port);

/*
 * Espera y acepta una conexion entrante.
 * Devuelve el socket de comunicacion con el cliente,
 * o INVALID_SOCKET si falla.
 *
 * Uso:
 *   SOCKET comm_socket = server_accept(conn_socket);
 */
SOCKET server_accept(SOCKET conn_socket);

/*
 * Cierra todos los sockets y limpia Winsock.
 * Llamar al final del programa o cuando el cliente se desconecta.
 */
void server_close(SOCKET conn_socket, SOCKET comm_socket);

#endif /* SERVER_SOCKET_H_ */
