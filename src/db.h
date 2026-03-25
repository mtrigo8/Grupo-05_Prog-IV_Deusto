/*
 * db.h
 *
 *  Created on: 25 mar 2026
 *      Author: markel.trigo
 */

#ifndef DB_H_
#define DB_H_

int registrar_log(sqlite3 *db, int id_usuario, const char *nivel, const char *mensaje);

#endif /* DB_H_ */
