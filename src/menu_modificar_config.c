/*
 * menu_modificar_config.c
 *
 * Vista y logica del menu de edicion de la configuracion del sistema.
 */

#include "../server/menu_modificar_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../server/config.h"
#include "../server/hash.h"
#include "../server/sqlite3.h"

/* ── Vista ── */
void crearMenuModificarConfiguracion(Config c) {
    printf("======================= \n");
    printf("Modificar configuracion \n");
    printf("DB path: %s\n",                    c.db_path);
    printf("Admin DNI (hash): %s\n",           c.admin_dni);
    printf("Admin password (hash): %s\n",      c.admin_password);
    printf("Log path: %s\n",                   c.log_path);
    printf("Numero maximo de negocios: %d\n",  c.max_negocios);
    printf("Pulse Enter al finalizar \n");
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuModificarConfig(sqlite3 *db, Config *c_actual) {
    Config c_nueva;
    memset(&c_nueva, 0, sizeof(Config));

    /* DB path */
    crearMenuModificarConfiguracion(c_nueva);
    fflush(stdout);
    { char _buf[270]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%255[^\n]", c_nueva.db_path); fflush(stdin); }

    /* DNI del admin (se guarda hasheado) */
    char dni_admin[20];
    crearMenuModificarConfiguracion(c_nueva);
    fflush(stdout);
    { char _buf[32]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%19s", dni_admin); fflush(stdin); }
    sha256_hex(dni_admin, c_nueva.admin_dni);

    /* Contrasenya del admin (se guarda hasheada) */
    char pass_plana[65];
    crearMenuModificarConfiguracion(c_nueva);
    fflush(stdout);
    { char _buf[80]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%49s", pass_plana); fflush(stdin); }
    sha256_hex(pass_plana, c_nueva.admin_password);

    /* Log path */
    crearMenuModificarConfiguracion(c_nueva);
    fflush(stdout);
    { char _buf[270]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%255[^\n]", c_nueva.log_path); fflush(stdin); }

    /* Maximo de negocios */
    crearMenuModificarConfiguracion(c_nueva);
    fflush(stdout);
    {
        char _buf[16];
        fgets(_buf, sizeof(_buf), stdin);
        fflush(stdin);
        if (sscanf(_buf, "%d", &c_nueva.max_negocios) != 1)
            c_nueva.max_negocios = c_actual->max_negocios;
    }

    crearMenuModificarConfiguracion(c_nueva);
    printf("\nGuardando cambios...\n");
    config_guardar(&c_nueva);

    printf("Pulse Enter para continuar...\n");
    fflush(stdout);
    getchar();
}
