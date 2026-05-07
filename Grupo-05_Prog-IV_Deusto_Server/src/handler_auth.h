#ifndef HANDLER_AUTH_H_
#define HANDLER_AUTH_H_

#include <winsock2.h>
#include "sqlite3.h"

void handler_auth_login(SOCKET comm_socket, sqlite3 *db, const char *params);

void handler_auth_register(SOCKET comm_socket, sqlite3 *db, const char *params);

void handler_auth_logout(SOCKET comm_socket, sqlite3 *db, const char *params);

#endif /* HANDLER_AUTH_H_ */
