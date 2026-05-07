#ifndef HANDLER_SERVICIOS_H_
#define HANDLER_SERVICIOS_H_

#include <winsock2.h>
#include "sqlite3.h"


void handler_servicios_get_all(SOCKET comm_socket, sqlite3 *db, const char *params);

void handler_servicios_get_one(SOCKET comm_socket, sqlite3 *db, const char *params);

void handler_servicios_filter(SOCKET comm_socket, sqlite3 *db, const char *params);

#endif /* HANDLER_SERVICIOS_H_ */
