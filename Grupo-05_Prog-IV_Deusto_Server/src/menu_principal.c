/*
 * menu_principal.c
 *
 * Vista y logica del menu principal (post-login de administrador).
 */

#include "menu_principal.h"

#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "usuario.h"
#include "menu_config.h"
#include "menu_negocios.h"
#include "sqlite3.h"

/* ── Vista ── */
void crearMenuPrincipal(void) {
    printf("======================= \n");
    printf("1. Gestionar negocios \n");
    printf("2. Gestionar configuracion \n");
    printf("3. Salir \n");
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuPrincipal(sqlite3 *db, Usuario u_final, Config *c) {
    int opcion = 0;
    int salir  = 0;

    while (!salir) {
        crearMenuPrincipal();
        fflush(stdout);

        char _buf[16];
        fgets(_buf, sizeof(_buf), stdin);
        fflush(stdin);
        if (sscanf(_buf, "%d", &opcion) != 1) opcion = 0;

        switch (opcion) {
            case 1: gestionMenuNegocios(db, u_final); break;
            case 2: gestionMenuConfig(db, c);         break;
            case 3: salir = 1;                        break;
            default: printf("Opcion invalida \n\n");
        }
    }
}
