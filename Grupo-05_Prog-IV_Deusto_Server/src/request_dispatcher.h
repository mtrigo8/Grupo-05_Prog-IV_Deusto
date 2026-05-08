
#ifndef REQUEST_DISPATCHER_H_
#define REQUEST_DISPATCHER_H_

#include <winsock2.h>
#include "sqlite3.h"


int dispatch(SOCKET comm_socket, sqlite3 *db, char *mensaje);

#endif /* REQUEST_DISPATCHER_H_ */
