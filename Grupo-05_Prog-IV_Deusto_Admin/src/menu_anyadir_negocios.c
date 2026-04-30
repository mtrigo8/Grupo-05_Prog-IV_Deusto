/*
 * menu_anyadir_negocios.c
 *
 * Vista y logica del menu de alta de un nuevo negocio.
 */

#include "menu_anyadir_negocios.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "negocio.h"
#include "sqlite3.h"

/* ── Vista ── */
void crearMenuAnyadirNegocios(Negocio n) {
    printf("======================= \n");
    printf("Anyadir negocio\n");
    printf("Nombre del negocio: %s\n",         n.nombre        ? n.nombre        : "");
    printf("Municipio: %s\n",                  n.municipio     ? n.municipio     : "");
    printf("Hora apertura (hh:mm): %s\n",      n.hora_apertura ? n.hora_apertura : "");
    printf("Hora cierre  (hh:mm): %s\n",       n.hora_cierre   ? n.hora_cierre   : "");
    printf("Tipo de negocio: %s\n",            n.tipo          ? n.tipo          : "");
    printf("Dias abierto separados por comas: %s\n", n.dias   ? n.dias           : "");
    printf("Pulse Enter al finalizar \n");
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuAnyadirNegocios(sqlite3 *db) {
    Negocio n;
    memset(&n, 0, sizeof(Negocio));
    /* Todos los punteros son NULL; se iran asignando con strdup */

    char tmp[1024]; /* buffer temporal de lectura */

    /* Nombre */
    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[80];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%74[^\n]", tmp);
        free(n.nombre);
        n.nombre = strdup(tmp);
        fflush(stdin);
    }

    /* Municipio */
    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%49[^\n]", tmp);
        free(n.municipio);
        n.municipio = strdup(tmp);
        fflush(stdin);
    }

    /* Hora apertura */
    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%19s", tmp);
        free(n.hora_apertura);
        n.hora_apertura = strdup(tmp);
        fflush(stdin);
    }

    /* Hora cierre */
    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%19s", tmp);
        free(n.hora_cierre);
        n.hora_cierre = strdup(tmp);
        fflush(stdin);
    }

    /* Tipo */
    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%49[^\n]", tmp);
        free(n.tipo);
        n.tipo = strdup(tmp);
        fflush(stdin);
    }

    /* Dias */
    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[270];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%255[^\n]", tmp);
        free(n.dias);
        n.dias = strdup(tmp);
        n.fecha = convertirDiasInt(tmp);
        fflush(stdin);
    }

    int res = insert_negocio(db, n);

    if (res == SQLITE_DONE) {
        printf("\n¡Negocio '%s' anyadido con exito a la base de datos!\n",
               n.nombre ? n.nombre : "");
    } else {
        printf("\nError: No se pudo anyadir (¿Quizas ese nombre ya existe?).\n");
    }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    getchar();

    /* Liberar todos los campos dinamicos del negocio */
    negocio_free(&n);
}
