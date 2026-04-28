/*
 * usuario.h
 *
 *  Estructura Usuario y funciones de acceso a la base de datos
 *  relacionadas con usuarios (registro e inicio de sesion).
 */

#ifndef USUARIO_H_
#define USUARIO_H_

#include "../server/sqlite3.h"

typedef struct {
    char nombre[50];
    char apellido[50];
    char dni[20];
    char contrasena[65];
} Usuario;

/*
 * Inserta un nuevo usuario en la base de datos.
 * datos[0]=nombre, datos[1]=apellido, datos[2]=dni, datos[3]=password_hash
 * Devuelve SQLITE_OK si tiene exito.
 */
int insert_usuario(sqlite3 *db, char *datos[]);

/*
 * Busca un usuario por DNI y hash de contrasena.
 * Devuelve el Usuario encontrado; si no existe, u.nombre[0] == '\0'.
 */
Usuario login_usuario(sqlite3 *db, char *dni, char *contrasena);

#endif /* USUARIO_H_ */
