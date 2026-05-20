/*
 * negocio.h
 *
 *  Estructura Negocio, enumeracion Dias y funciones de conversion
 *  de dias + operaciones CRUD sobre la tabla servicio en la BD.
 *
 *  NOTA DE MEMORIA:
 *    Todos los campos de cadena en Negocio son punteros dinamicos.
 *    Usar negocio_free() para liberar cada instancia cuando ya no se necesite.
 *    El array devuelto por get_negocios() debe liberarse con:
 *      for (int i = 0; i < total; i++) negocio_free(&lista[i]);
 *      free(lista);
 */

#ifndef NEGOCIO_H_
#define NEGOCIO_H_

#include "sqlite3.h"

typedef struct {
    char *nombre;           /* malloc — max ~74 chars util */
    char *municipio;        /* malloc — max ~49 chars util */
    char *hora_apertura;    /* malloc — max ~19 chars util */
    char *hora_cierre;      /* malloc — max ~19 chars util */
    int   fecha;            /* mascara de bits, valor fijo */
    char *tipo;             /* malloc — max ~49 chars util */
    char *dias;             /* malloc — dias en string, provisional */
    char *descripcion;      /* malloc — puede ser largo (hasta ~999 chars) */
    int   capacidad_max;    /* FIX: capacidad maxima del servicio */
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

/* ── Gestion de memoria ── */

void negocio_free(Negocio *n);

/* ── Conversion de dias ── */

int  convertirDiasInt(char dias[]);
void convertirIntDias(int mascara, char resultado[]);

/* ── Operaciones CRUD en base de datos ── */

Negocio *get_negocios(sqlite3 *db, int *total_negocios);
int insert_negocio(sqlite3 *db, Negocio n);
int delete_negocio(sqlite3 *db, char *nombre);
int update_negocio(sqlite3 *db, char *nombre_actual, Negocio n_nuevo);

#endif /* NEGOCIO_H_ */
