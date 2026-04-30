/*
 * menu_eliminar_negocios.c
 *
 * Vista y logica del menu de eliminacion de un negocio.
 */

#include "menu_eliminar_negocios.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "negocio.h"
#include "sqlite3.h"

/* ── Vista: formulario de busqueda ── */
void crearMenuEliminarNegocios(Negocio n) {
    printf("======================= \n");
    printf("Eliminar negocio \n");
    printf("Negocio: %s\n",   n.nombre    ? n.nombre    : "");
    printf("Municipio: %s\n", n.municipio ? n.municipio : "");
    printf("Pulse Enter al finalizar \n");
    printf("======================= \n");
}

/* ── Vista: confirmacion ── */
void crearMenuEliminarNegociosConfirm(Negocio n) {
    printf("======================= \n");
    printf("Eliminar negocio \n");
    printf("Seguro que quieres eliminar el negocio '%s' en '%s'? \n",
           n.nombre    ? n.nombre    : "",
           n.municipio ? n.municipio : "");
    printf("[s/n]: ");
    fflush(stdout);
    printf("\n======================= \n");
}

/* ── Logica ── */
void gestionMenuEliminarNegocios(sqlite3 *db) {
    Negocio n_vacia;
    memset(&n_vacia, 0, sizeof(Negocio));
    /* Todos los punteros son NULL */

    char tmp[256];

    /* Nombre del negocio a eliminar */
    crearMenuEliminarNegocios(n_vacia);
    fflush(stdout);
    {
        char _buf[80];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%74[^\n]", tmp);
        free(n_vacia.nombre);
        n_vacia.nombre = strdup(tmp);
        fflush(stdin);
    }

    /* Municipio (para confirmacion visual) */
    crearMenuEliminarNegocios(n_vacia);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%49[^\n]", tmp);
        free(n_vacia.municipio);
        n_vacia.municipio = strdup(tmp);
        fflush(stdin);
    }

    crearMenuEliminarNegociosConfirm(n_vacia);

    char confirmacion;
    { char _buf[8]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, " %c", &confirmacion); fflush(stdin); }

    if (confirmacion == 's' || confirmacion == 'S') {
        int res = delete_negocio(db, n_vacia.nombre);

        if (res == SQLITE_DONE) {
            if (sqlite3_changes(db) > 0)
                printf("\n¡Se ha eliminado el negocio '%s'!\n",
                       n_vacia.nombre ? n_vacia.nombre : "");
            else
                printf("\nNo se encontro ningun negocio llamado '%s'.\n",
                       n_vacia.nombre ? n_vacia.nombre : "");
        } else {
            printf("\nHubo un problema al intentar eliminar en la base de datos.\n");
        }
    } else {
        printf("\nOperacion cancelada. El negocio no ha sido borrado.\n");
    }

    printf("Presione Enter para volver...");
    fflush(stdout);
    getchar();

    /* Liberar los campos dinamicos del negocio temporal */
    negocio_free(&n_vacia);
}
