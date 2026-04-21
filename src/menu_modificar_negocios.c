/*
 * menu_modificar_negocios.c
 *
 * Vista y logica del menu de modificacion de un negocio existente.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "estructuras.h"
#include "db.h"
#include "menu_modificar_negocios.h"

/* ── Vista ── */
void crearMenuModificarNegocios(char *nombreActual, Negocio n) {
    printf("======================= \n");
    printf("Modificar negocio \n");
    printf("Nombre actual: %s\n",          nombreActual);
    printf("Nuevo nombre: %s\n",           n.nombre);
    printf("Municipio: %s\n",             n.municipio);
    printf("Hora apertura (hh:mm): %s\n", n.hora_apertura);
    printf("Hora cierre  (hh:mm): %s\n",  n.hora_cierre);
    printf("Tipo de negocio: %s\n",        n.tipo);
    printf("Dias abierto: %s\n",           n.dias);
    printf("Pulse Enter al finalizar \n");
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuModificarNegocios(sqlite3 *db) {
    char nombre_actual[75];
    nombre_actual[0] = '\0';
    Negocio n_nuevo;
    memset(&n_nuevo, 0, sizeof(Negocio));

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    { char _buf[80];  fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%74[^\n]", nombre_actual);       fflush(stdin); }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    { char _buf[80];  fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%74[^\n]", n_nuevo.nombre);      fflush(stdin); }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    { char _buf[64];  fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%49[^\n]", n_nuevo.municipio);   fflush(stdin); }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    { char _buf[32];  fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%19s",     n_nuevo.hora_apertura); fflush(stdin); }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    { char _buf[32];  fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%19s",     n_nuevo.hora_cierre);  fflush(stdin); }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    { char _buf[64];  fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%49[^\n]", n_nuevo.tipo);         fflush(stdin); }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    { char _buf[270]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%255[^\n]", n_nuevo.dias);        fflush(stdin); }
    n_nuevo.fecha = convertirDiasInt(n_nuevo.dias);

    int res = update_negocio(db, nombre_actual, n_nuevo);

    if (res == SQLITE_DONE && sqlite3_changes(db) > 0)
        printf("\n¡Negocio '%s' actualizado con exito!\n", nombre_actual);
    else
        printf("\nNo se pudo actualizar (¿Seguro que el negocio '%s' existe?).\n", nombre_actual);

    fflush(stdout);
    getchar();
}
