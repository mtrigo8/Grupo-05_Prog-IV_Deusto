/*
 * menu_registro.c
 *
 * Vista y logica del menu de registro de nuevos usuarios.
 */

#include "menu_registro.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "usuario.h"
#include "sqlite3.h"

/* ── Vista ── */
void crearMenuRegistro(Usuario u) {
    printf("======================= \n");
    printf("Registro \n");
    printf("Nombre: %s \n",        u.nombre   ? u.nombre   : "");
    printf("Apellido: %s \n",      u.apellido ? u.apellido : "");
    printf("Usuario (DNI): %s \n", u.dni      ? u.dni      : "");
    printf("Contrasenya: %s \n",   u.contrasena[0] != '\0' ? "********" : "");
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuRegistro(sqlite3 *db) {
    Usuario u_temp;
    memset(&u_temp, 0, sizeof(Usuario));
    /* nombre, apellido, dni son NULL; contrasena[65] a ceros */

    char tmp[128]; /* buffer temporal de lectura */

    /* Nombre */
    crearMenuRegistro(u_temp);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%49s", tmp);
        free(u_temp.nombre);
        u_temp.nombre = strdup(tmp);
        fflush(stdin);
    }

    /* Apellido */
    crearMenuRegistro(u_temp);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%49s", tmp);
        free(u_temp.apellido);
        u_temp.apellido = strdup(tmp);
        fflush(stdin);
    }

    /* DNI */
    crearMenuRegistro(u_temp);
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

    /* Contrasenya en texto plano → se almacena en el array fijo para hashear */
    crearMenuRegistro(u_temp);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        tmp[0] = '\0';
        sscanf(_buf, "%49s", tmp);
        strncpy(u_temp.contrasena, tmp, sizeof(u_temp.contrasena) - 1);
        fflush(stdin);
    }

    /* Hashear la contrasenya antes de insertar */
    char hash[65];
    sha256_hex(u_temp.contrasena, hash);

    char *datos[4] = { u_temp.nombre, u_temp.apellido, u_temp.dni, hash };
    int res = insert_usuario(db, datos);

    if (res == SQLITE_OK) {
        printf("\nRegistro completado con exito.\n");
        printf("El apartado de consumidor no esta disenado aun, intentelo mas adelante.\n");
    } else {
        printf("\nError al registrar: el DNI ya existe o hubo un problema con la base de datos.\n");
    }
    printf("Presione Enter para volver al inicio...");
    fflush(stdout);
    getchar();

    /* Liberar los campos dinamicos del usuario temporal */
    usuario_free(&u_temp);
}
