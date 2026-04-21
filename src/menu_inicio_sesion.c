/*
 * menu_inicio_sesion.c
 *
 * Vista y logica del menu de inicio de sesion.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "estructuras.h"
#include "config.h"
#include "db.h"
#include "hash.h"
#include "menu_inicio_sesion.h"
#include "menu_principal.h"

/* ── Vista ── */
void crearMenuInicioSesion(Usuario u) {
    printf("======================= \n");
    printf("Inicio de sesion \n");
    printf("Usuario (DNI): %s \n", u.dni);
    printf("Contrasenya: %s \n",   u.contrasena);
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuInicioSesion(sqlite3 *db, Config *c) {
    Usuario u_temp;
    memset(&u_temp, 0, sizeof(Usuario));

    /* Pedir DNI */
    crearMenuInicioSesion(u_temp);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%19s", u_temp.dni);
        fflush(stdin);
    }

    /* Pedir contrasenya */
    crearMenuInicioSesion(u_temp);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%49s", u_temp.contrasena);
        fflush(stdin);
    }

    /* Comprobar credenciales de administrador */
    char hash_dni[65];
    char hash_pass[65];
    sha256_hex(u_temp.dni,        hash_dni);
    sha256_hex(u_temp.contrasena, hash_pass);

    if (strcmp(hash_dni,  c->admin_dni)      == 0 &&
        strcmp(hash_pass, c->admin_password) == 0) {

        printf("\nAcceso concedido. Bienvenido, administrador.\n");
        registrar_log(db, 0, "INFO", "Inicio de sesion de administrador correcto");

        Usuario admin;
        memset(&admin, 0, sizeof(Usuario));
        strncpy(admin.nombre, "Admin",    sizeof(admin.nombre)   - 1);
        strncpy(admin.dni,    u_temp.dni, sizeof(admin.dni)      - 1);

        gestionMenuPrincipal(db, admin, c);
        return;
    }

    /* Login de usuario normal en BD */
    Usuario u_final = login_usuario(db, u_temp.dni, hash_pass);

    if (u_final.nombre[0] != '\0') {
        printf("\nBienvenido, %s.\n", u_final.nombre);
        printf("El apartado de consumidor no esta disenado aun, intentelo mas adelante.\n");
        printf("Presione Enter para volver al inicio...");
        fflush(stdout);
        getchar();
    } else {
        printf("\nError: Usuario o contrasenya incorrectos.\n");
        printf("Presione Enter para volver...");
        fflush(stdout);
        getchar();
    }
}
