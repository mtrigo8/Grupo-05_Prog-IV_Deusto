/*
 * menu_bienvenida.c
 *
 * Vista y logica del menu de bienvenida (pantalla inicial).
 */

#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "config.h"
#include "menu_bienvenida.h"
#include "menu_inicio_sesion.h"
#include "menu_registro.h"

/* ── Vista ── */
void crearMenuBienvenida() {
    printf("======================= \n");
    printf("  ____ _ _         _   _       _      \n");
    printf(" / ___(_) |_ _   _| | | |_   _| |__   \n");
    printf("| |   | | __| | | | |_| | | | | '_ \\ \n");
    printf("| |___| | |_| |_| |  _  | |_| | |_) | \n");
    printf(" \\____|_|\\__|\\__, |_| |_|\\__,_|_.__/  \n");
    printf("             |___/                    \n");
    printf("======================= \n");
    printf("Bienvenido a CityHub \n");
    printf("1. Inicio sesion \n");
    printf("2. Registrarse \n");
    printf("3. Salir \n");
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuBienvenida(sqlite3 *db, Config *c) {
    int opcion = 0;
    int salir  = 0;

    while (!salir) {
        crearMenuBienvenida();
        fflush(stdout);

        char _buf[16];
        fgets(_buf, sizeof(_buf), stdin);
        fflush(stdin);
        if (sscanf(_buf, "%d", &opcion) != 1) opcion = 0;

        switch (opcion) {
            case 1: gestionMenuInicioSesion(db, c); break;
            case 2: gestionMenuRegistro(db);         break;
            case 3: salir = 1;                       break;
            default: printf("Opcion invalida \n\n");
        }
    }
}
