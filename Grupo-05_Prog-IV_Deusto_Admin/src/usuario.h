/*
 * usuario.h
 *
 *  Estructura Usuario y funciones de acceso a la base de datos
 *  relacionadas con usuarios (registro e inicio de sesion).
 *
 *  NOTA DE MEMORIA:
 *    nombre, apellido y dni son punteros dinamicos → usar usuario_free().
 *    contrasena[65] es un array fijo porque siempre almacena un hash
 *    SHA-256 de longitud constante (64 hex + '\0').
 */

#ifndef USUARIO_H_
#define USUARIO_H_

#include "sqlite3.h"

typedef struct {
    char *nombre;         /* malloc */
    char *apellido;       /* malloc */
    char *dni;            /* malloc */
    char  contrasena[65]; /* array fijo: hash SHA-256 (64 hex + '\0') */
} Usuario;

/* Libera los campos dinamicos de un Usuario y los pone a NULL.
 * contrasena es un array fijo y NO se libera. */
void usuario_free(Usuario *u);

/*
 * Inserta un nuevo usuario en la base de datos.
 * datos[0]=nombre, datos[1]=apellido, datos[2]=dni, datos[3]=password_hash
 * Devuelve SQLITE_OK si tiene exito.
 */
int insert_usuario(sqlite3 *db, char *datos[]);

/*
 * Busca un usuario por DNI y hash de contrasena.
 * Devuelve el Usuario encontrado con campos asignados dinamicamente;
 * si no existe, u.nombre == NULL.
 * El llamador debe liberar el resultado con usuario_free().
 */
Usuario login_usuario(sqlite3 *db, char *dni, char *contrasena);

#endif /* USUARIO_H_ */
