

#ifndef HANDLER_RESERVAS_H_
#define HANDLER_RESERVAS_H_

#include <winsock2.h>
#include "sqlite3.h"


typedef struct {
    int  id_reserva;
    int  id_usuario;
    int  id_servicio;
    char fecha_registro[20];
    char nombre_servicio[75];
} Reserva;

void handler_create_reserva(SOCKET comm_socket, sqlite3 *db, char *params);

void handler_get_reserva(SOCKET comm_socket, sqlite3 *db, char *params);

void handler_cancel_reserva(SOCKET comm_socket, sqlite3 *db, char *params);

void handler_update_reserva(SOCKET comm_socket, sqlite3 *db, char *params);

#endif /* HANDLER_RESERVAS_H_ */
