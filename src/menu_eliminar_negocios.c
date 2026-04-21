/*
 * menu_eliminar_negocios.c
 *
 * Vista y logica del menu de eliminacion de un negocio.
 * Incluye la pantalla de confirmacion previa al borrado.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "estructuras.h"
#include "db.h"
#include "menu_eliminar_negocios.h"

/* ── Vista: formulario de busqueda ── */
void crearMenuEliminarNegocios(Negocio n) {
    printf("======================= \n");
    printf("Eliminar negocio \n");
    printf("Negocio: %s\n",   n.nombre);
    printf("Municipio: %s\n", n.municipio);
    printf("Pulse Enter al finalizar \n");
    printf("======================= \n");
}

/* ── Vista: confirmacion ── */
void crearMenuEliminarNegociosConfirm(Negocio n) {
    printf("======================= \n");
    printf("Eliminar negocio \n");
    printf("Seguro que quieres eliminar el negocio '%s' en '%s'? \n",
           n.nombre, n.municipio);
    printf("[s/n]: ");
    fflush(stdout);
    printf("\n======================= \n");
}

/* ── Logica ── */
void gestionMenuEliminarNegocios(sqlite3 *db) {
    Negocio n_vacia;
    memset(&n_vacia, 0, sizeof(Negocio));

    crearMenuEliminarNegocios(n_vacia);
    fflush(stdout);
    { char _buf[80]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%74[^\n]", n_vacia.nombre);    fflush(stdin); }

    crearMenuEliminarNegocios(n_vacia);
    fflush(stdout);
    { char _buf[64]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%49[^\n]", n_vacia.municipio); fflush(stdin); }

    crearMenuEliminarNegociosConfirm(n_vacia);

    char confirmacion;
    { char _buf[8]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, " %c", &confirmacion); fflush(stdin); }

    if (confirmacion == 's' || confirmacion == 'S') {
        int res = delete_negocio(db, n_vacia.nombre);

        if (res == SQLITE_DONE) {
            if (sqlite3_changes(db) > 0)
                printf("\n¡Se ha eliminado el negocio '%s'!\n", n_vacia.nombre);
            else
                printf("\nNo se encontro ningun negocio llamado '%s'.\n", n_vacia.nombre);
        } else {
            printf("\nHubo un problema al intentar eliminar en la base de datos.\n");
        }
    } else {
        printf("\nOperacion cancelada. El negocio no ha sido borrado.\n");
    }

    printf("Presione Enter para volver...");
    fflush(stdout);
    getchar();
}
