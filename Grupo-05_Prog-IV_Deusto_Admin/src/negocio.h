/*
 * negocio.h
 *
 *  Estructura Negocio, enumeracion Dias y funciones de conversion
 *  de dias + operaciones CRUD sobre la tabla servicio en la BD.
 */

#ifndef NEGOCIO_H_
#define NEGOCIO_H_

#include "sqlite3.h"

typedef struct {
    char nombre[75];
    char municipio[50];
    char hora_apertura[20];
    char hora_cierre[20];
    int  fecha;           /* dias en mascara de bits */
    char tipo[50];
    char dias[50];        /* dias en string, provisional */
    char descripcion[1000];
} Negocio;

enum Dias {
    LUNES     = 1 << 0,  /* 1  */
    MARTES    = 1 << 1,  /* 2  */
    MIERCOLES = 1 << 2,  /* 4  */
    JUEVES    = 1 << 3,  /* 8  */
    VIERNES   = 1 << 4,  /* 16 */
    SABADO    = 1 << 5,  /* 32 */
    DOMINGO   = 1 << 6   /* 64 */
};

/* ── Conversion de dias ── */

/* Convierte una cadena de dias separados por comas a mascara de bits */
int  convertirDiasInt(char dias[]);

/* Convierte una mascara de bits a cadena de dias separados por comas */
void convertirIntDias(int mascara, char resultado[]);

/* ── Operaciones CRUD en base de datos ── */

/* Obtiene todos los negocios de la BD; el llamador debe liberar el array */
Negocio *get_negocios(sqlite3 *db, int *total_negocios);

/* Inserta un nuevo negocio; devuelve SQLITE_DONE si tiene exito */
int insert_negocio(sqlite3 *db, Negocio n);

/* Elimina el negocio con ese nombre; devuelve SQLITE_DONE si tiene exito */
int delete_negocio(sqlite3 *db, char *nombre);

/* Actualiza los datos de un negocio existente; devuelve SQLITE_DONE si exito */
int update_negocio(sqlite3 *db, char *nombre_actual, Negocio n_nuevo);

#endif /* NEGOCIO_H_ */
