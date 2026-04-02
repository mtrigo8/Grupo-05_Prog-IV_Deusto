/*
 * db.h
 *
 *  Created on: 25 mar 2026
 *      Author: markel.trigo
 */

#ifndef DB_H_
#define DB_H_
#include "sqlite3.h"
#include "estructuras.h"

int registrar_log(sqlite3 *db, int id_usuario, const char *nivel, const char *mensaje);
int borrar_reservas(sqlite3 *db);
int insert_usuario(sqlite3 *db, char *datos[]);
Usuario login_usuario(sqlite3 *db, char *nombre, char *contrasena);


#endif /* DB_H_ */
