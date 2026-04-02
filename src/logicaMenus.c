#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "estructuras.h"
#include "menus.h"
#include "db.h"
#include "logicaMenus.h"

void gestionMenuBienvenida(sqlite3 *db){
	int opcion = 0;
	int salir = 0;

	while (!salir){
		crearMenuBienvenida();
		fflush(stdout);

		if (scanf(" %d", &opcion) != 1) {
		   while(getchar() != '\n');
		   opcion = 0;
		        }

		switch (opcion) {
			case 1:
				gestionMenuInicioSesion(db);
				break;
			case 2:
				gestionMenuRegistro(db);
				break;
			case 3:
				salir = 1;
				break;
			default:
				printf("Opción invalida \n\n");
		}
	}
}

void gestionMenuInicioSesion(sqlite3 *db) {
    Usuario u_temp;
    // Iniciar el usuario temporal en vacío
    memset(&u_temp, 0, sizeof(Usuario));

    // 1. Pedir DNI
    crearMenuInicioSesion(u_temp);
    fflush(stdout);
    scanf(" %19s", u_temp.dni); // %19s para no desbordar los 20 caracteres del struct

    // 2. Pedir la Contraseña (re-printeamos el menú con el DNI ya puesto)
    crearMenuInicioSesion(u_temp);
    fflush(stdout);
    scanf(" %49s", u_temp.contrasena);

    Usuario u_final = login_usuario(db, u_temp.dni, u_temp.contrasena);

    if (strcmp(u_final.nombre,"") != 0) {
        // LOGIN CORRECTO
        printf("\nAcceso concedido. Bienvenido, %s.\n", u_final.nombre);
        	// Aquí llamar a: gestionMenuPrincipal(db, u_final);
    } else {
        // LOGIN FALLIDO
        printf("\nError: Usuario o contraseña incorrectos.\n");
        printf("Presione Enter para volver...");
        fflush(stdout);
        while(getchar() != '\n'); // Limpia el buffer
        getchar();
    }
}
void gestionMenuRegistro(sqlite3 *db){

}


