/*
 * log.h
 *
 *  Utilidades de logging para CityHub:
 *    - log_init      : establece la ruta del fichero de log en disco
 *    - registrar_log : graba un evento en la tabla log de la BD
 *                      y lo escribe tambien en el fichero de log
 */

#ifndef LOG_H_
#define LOG_H_

#include "../server/sqlite3.h"

/* Establece la ruta del fichero de log en disco */
void log_init(const char *path);

/* Registra un evento en la tabla log de la BD y en el fichero de log */
int registrar_log(sqlite3 *db, int id_usuario, const char *nivel, const char *mensaje);

#endif /* LOG_H_ */
