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
    printf("Nombre del negocio: %s\n",         n.nombre);
    printf("Municipio: %s\n",                  n.municipio);
    printf("Hora apertura (hh:mm): %s\n",      n.hora_apertura);
    printf("Hora cierre  (hh:mm): %s\n",       n.hora_cierre);
    printf("Tipo de negocio: %s\n",            n.tipo);
    printf("Dias abierto separados por comas: %s\n", n.dias);
    printf("Pulse Enter al finalizar \n");
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuAnyadirNegocios(sqlite3 *db) {
    Negocio n;
    memset(&n, 0, sizeof(Negocio));

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    { char _buf[80];  fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%74[^\n]", n.nombre);        fflush(stdin); }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    { char _buf[64];  fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%49[^\n]", n.municipio);     fflush(stdin); }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    { char _buf[32];  fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%19s",     n.hora_apertura); fflush(stdin); }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    { char _buf[32];  fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%19s",     n.hora_cierre);   fflush(stdin); }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    { char _buf[64];  fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%49[^\n]", n.tipo);          fflush(stdin); }

    char dias_temp[256];
    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    { char _buf[270]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%255[^\n]", dias_temp);      fflush(stdin); }
    n.fecha = convertirDiasInt(dias_temp);

    int res = insert_negocio(db, n);

    if (res == SQLITE_DONE) {
        printf("\n¡Negocio '%s' anyadido con exito a la base de datos!\n", n.nombre);
    } else {
        printf("\nError: No se pudo anyadir (¿Quizas ese nombre ya existe?).\n");
    }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    getchar();
}
