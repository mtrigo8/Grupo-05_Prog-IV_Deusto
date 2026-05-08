

#ifndef HANDLER_CONEXION_H_
#define HANDLER_CONEXION_H_

#include <winsock2.h>
#include "sqlite3.h"


void handler_ping(SOCKET comm_socket);

int handler_disconnect(SOCKET comm_socket, sqlite3 *db, char *params);

#endif /* HANDLER_CONEXION_H_ */
