/*
 * menu_negocios.c
 *
 * Vista y logica del menu de gestion de negocios (submenu).
 */

#include "menu_negocios.h"

#include <stdio.h>
#include <stdlib.h>

#include "usuario.h"
#include "menu_anyadir_negocios.h"
#include "menu_eliminar_negocios.h"
#include "menu_modificar_negocios.h"
#include "menu_ver_negocios.h"
#include "sqlite3.h"

/* ── Vista ── */
void crearMenuNegocios(void) {
    printf("======================= \n");
    printf("1. Ver negocios \n");
    printf("2. Anyadir negocios \n");
    printf("3. Eliminar negocios \n");
    printf("4. Modificar negocios \n");
    printf("5. Volver \n");
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuNegocios(sqlite3 *db, Usuario u_final) {
    int opcion = 0;
    int salir  = 0;

    (void)u_final; /* reservado para control de permisos futuro */

    while (!salir) {
        crearMenuNegocios();
        fflush(stdout);

        char _buf[16];
        fgets(_buf, sizeof(_buf), stdin);
        fflush(stdin);
        if (sscanf(_buf, "%d", &opcion) != 1) opcion = 0;

        switch (opcion) {
            case 1: gestionMenuVerNegocios(db);       break;
            case 2: gestionMenuAnyadirNegocios(db);   break;
            case 3: gestionMenuEliminarNegocios(db);  break;
            case 4: gestionMenuModificarNegocios(db); break;
            case 5: salir = 1;                        break;
            default: printf("Opcion invalida \n\n");
        }
    }
}
