/*
 * menu_inicio_sesion.c
 *
 * Vista y logica del menu de inicio de sesion.
 */

#include "menu_inicio_sesion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "log.h"
#include "hash.h"
#include "menu_principal.h"
#include "usuario.h"
#include "sqlite3.h"

/* ── Vista ── */
void crearMenuInicioSesion(Usuario u) {
    printf("======================= \n");
    printf("Inicio de sesion \n");
    printf("Usuario (DNI): %s \n", u.dni ? u.dni : "");
    printf("Contrasenya: %s \n",   u.contrasena[0] != '\0' ? "********" : "");
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuInicioSesion(sqlite3 *db, Config *c) {
    Usuario u_temp;
    memset(&u_temp, 0, sizeof(Usuario));
    /* nombre, apellido, dni son NULL; contrasena[65] a ceros */

    char tmp[128]; /* buffer temporal de lectura */

    /* Pedir DNI */
    crearMenuInicioSesion(u_temp);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%19s", tmp);
        free(u_temp.dni);
        u_temp.dni = strdup(tmp);
        fflush(stdin);
    }

    /* Pedir contrasenya (en texto plano, se hashea despues) */
    crearMenuInicioSesion(u_temp);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%49s", tmp);
        /* La contrasenya en texto plano se guarda temporalmente en el array fijo */
        strncpy(u_temp.contrasena, tmp, sizeof(u_temp.contrasena) - 1);
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
        admin.nombre = strdup("Admin");
        admin.dni    = strdup(u_temp.dni);

        gestionMenuPrincipal(db, admin, c);

        /* Liberar el admin temporal tras volver del menu principal */
        usuario_free(&admin);

        /* Liberar u_temp y salir */
        usuario_free(&u_temp);
        return;
    }

    /* Login de usuario normal en BD */
    Usuario u_final = login_usuario(db, u_temp.dni, hash_pass);

    if (u_final.nombre != NULL) {
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

    /* Liberar estructuras dinamicas */
    usuario_free(&u_final);
    usuario_free(&u_temp);
}
