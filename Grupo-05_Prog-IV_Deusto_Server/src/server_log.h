/*
 * server_log.h
 *
 *  Log del servidor CityHub. Solo escribe en fichero (distinto al log.h
 *  del Admin, que ademas escribe en la tabla LOGS de la BD).
 *
 *  Este log registra eventos del servidor: arranque, conexiones,
 *  comandos recibidos y errores de red.
 *
 *  Uso:
 *    server_log_init("server.log");
 *    server_log("INFO",  "Servidor arrancado en puerto 6000");
 *    server_log("INFO",  "Cliente conectado: 127.0.0.1");
 *    server_log("ERROR", "recv() fallido");
 */

#ifndef SERVER_LOG_H_
#define SERVER_LOG_H_

/* Establece la ruta del fichero de log del servidor */
void server_log_init(const char *path);

/* Escribe una linea en el fichero de log con timestamp y nivel */
void server_log(const char *nivel, const char *mensaje);

#endif /* SERVER_LOG_H_ */
