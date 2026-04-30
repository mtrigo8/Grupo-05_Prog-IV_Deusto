/*
 * menu_modificar_negocios.c
 *
 * Vista y logica del menu de modificacion de un negocio existente.
 */

#include "menu_modificar_negocios.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "negocio.h"
#include "sqlite3.h"

/* ── Vista ── */
void crearMenuModificarNegocios(char *nombreActual, Negocio n) {
    printf("======================= \n");
    printf("Modificar negocio \n");
    printf("Nombre actual: %s\n",          nombreActual            ? nombreActual            : "");
    printf("Nuevo nombre: %s\n",           n.nombre                ? n.nombre                : "");
    printf("Municipio: %s\n",             n.municipio             ? n.municipio             : "");
    printf("Hora apertura (hh:mm): %s\n", n.hora_apertura         ? n.hora_apertura         : "");
    printf("Hora cierre  (hh:mm): %s\n",  n.hora_cierre           ? n.hora_cierre           : "");
    printf("Tipo de negocio: %s\n",        n.tipo                  ? n.tipo                  : "");
    printf("Dias abierto: %s\n",           n.dias                  ? n.dias                  : "");
    printf("Pulse Enter al finalizar \n");
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuModificarNegocios(sqlite3 *db) {
    /* nombre_actual es un buffer local de pila: solo se usa para leer
     * el nombre de busqueda y pasarlo como char* a las funciones. */
    char nombre_actual[75];
    nombre_actual[0] = '\0';

    Negocio n_nuevo;
    memset(&n_nuevo, 0, sizeof(Negocio));
    /* Todos los punteros son NULL */

    char tmp[1024]; /* buffer temporal de lectura */

    /* Nombre actual (clave de busqueda en BD) */
    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[80];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%74[^\n]", nombre_actual);
        fflush(stdin);
    }

    /* Nuevo nombre */
    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[80];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%74[^\n]", tmp);
        free(n_nuevo.nombre);
        n_nuevo.nombre = strdup(tmp);
        fflush(stdin);
    }

    /* Municipio */
    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%49[^\n]", tmp);
        free(n_nuevo.municipio);
        n_nuevo.municipio = strdup(tmp);
        fflush(stdin);
    }

    /* Hora apertura */
    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%19s", tmp);
        free(n_nuevo.hora_apertura);
        n_nuevo.hora_apertura = strdup(tmp);
        fflush(stdin);
    }

    /* Hora cierre */
    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%19s", tmp);
        free(n_nuevo.hora_cierre);
        n_nuevo.hora_cierre = strdup(tmp);
        fflush(stdin);
    }

    /* Tipo */
    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%49[^\n]", tmp);
        free(n_nuevo.tipo);
        n_nuevo.tipo = strdup(tmp);
        fflush(stdin);
    }

    /* Dias */
    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[270];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%255[^\n]", tmp);
        free(n_nuevo.dias);
        n_nuevo.dias  = strdup(tmp);
        n_nuevo.fecha = convertirDiasInt(tmp);
        fflush(stdin);
    }

    int res = update_negocio(db, nombre_actual, n_nuevo);

    if (res == SQLITE_DONE && sqlite3_changes(db) > 0)
        printf("\n¡Negocio '%s' actualizado con exito!\n", nombre_actual);
    else
        printf("\nNo se pudo actualizar (¿Seguro que el negocio '%s' existe?).\n",
               nombre_actual);

    fflush(stdout);
    getchar();

    /* Liberar todos los campos dinamicos del negocio nuevo */
    negocio_free(&n_nuevo);
}
