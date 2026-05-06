

#ifndef SERVER_SOCKET_H_
#define SERVER_SOCKET_H_

#include <winsock2.h>


SOCKET server_init(const char *ip, int port);
SOCKET server_accept(SOCKET conn_socket);
void server_close(SOCKET conn_socket, SOCKET comm_socket);

#endif /* SERVER_SOCKET_H_ */
