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
    printf("Nombre: %s \n",        u.nombre);
    printf("Apellido: %s \n",      u.apellido);
    printf("Usuario (DNI): %s \n", u.dni);
    printf("Contrasenya: %s \n",   u.contrasena);
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuRegistro(sqlite3 *db) {
    Usuario u_temp;
    memset(&u_temp, 0, sizeof(Usuario));

    crearMenuRegistro(u_temp);
    fflush(stdout);
    { char _buf[64]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%49s", u_temp.nombre);     fflush(stdin); }

    crearMenuRegistro(u_temp);
    fflush(stdout);
    { char _buf[64]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%49s", u_temp.apellido);   fflush(stdin); }

    crearMenuRegistro(u_temp);
    fflush(stdout);
    { char _buf[32]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%19s", u_temp.dni);        fflush(stdin); }

    crearMenuRegistro(u_temp);
    fflush(stdout);
    { char _buf[64]; fgets(_buf, sizeof(_buf), stdin); sscanf(_buf, "%49s", u_temp.contrasena); fflush(stdin); }

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
}
