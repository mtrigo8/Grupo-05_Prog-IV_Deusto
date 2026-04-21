/*
 * menu_config.c
 *
 * Vista y logica del menu de configuracion del sistema.
 */

#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "config.h"
#include "menu_config.h"
#include "menu_modificar_config.h"

/* ── Vista ── */
void crearMenuConfig() {
    printf("======================= \n");
    printf("1. Ver configuracion \n");
    printf("2. Modificar configuracion \n");
    printf("3. Volver \n");
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuConfig(sqlite3 *db, Config *c) {
    int opcion = 0;
    int salir  = 0;

    while (!salir) {
        crearMenuConfig();
        fflush(stdout);

        char _buf[16];
        fgets(_buf, sizeof(_buf), stdin);
        fflush(stdin);
        if (sscanf(_buf, "%d", &opcion) != 1) opcion = 0;

        switch (opcion) {
            case 1:
                config_mostrar(c);
                printf("Presione Enter para volver...\n");
                fflush(stdout);
                getchar();
                break;
            case 2:
                gestionMenuModificarConfig(db, c);
                break;
            case 3:
                salir = 1;
                break;
            default:
                printf("Opcion invalida \n\n");
        }
    }
}
