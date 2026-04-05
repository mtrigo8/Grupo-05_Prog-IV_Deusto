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
    	if (u_final.rol == 1){ //rol 1 = admin
    		printf("\nAcceso concedido. Bienvenido, %s.\n", u_final.nombre);
    		gestionMenuPrincipal(db, u_final);
    	}else{
    		printf("\nRegistro completado con éxito.\n");
    		        printf("El apartado de consumidor no esta diseñado aún, intentelo más adelante. \n Presione Enter para volver al inicio...");
    		        fflush(stdout);
    		        while(getchar() != '\n');
    		        getchar();
    	}
    } else {
        // LOGIN FALLIDO
        printf("\nError: Usuario o contraseña incorrectos.\n");
        printf("Presione Enter para volver...");
        fflush(stdout);
        while(getchar() != '\n'); // Limpia el buffer
        getchar();
    }
}

void gestionMenuRegistro(sqlite3 *db) {
    Usuario u_temp;
    memset(&u_temp, 0, sizeof(Usuario)); // Limpiar estructura

    crearMenuRegistro(u_temp);
    fflush(stdout);
    scanf(" %49s", u_temp.nombre);

    crearMenuRegistro(u_temp);
    fflush(stdout);
    scanf(" %49s", u_temp.apellido);

    crearMenuRegistro(u_temp);
    fflush(stdout);
    scanf(" %19s", u_temp.dni);

    crearMenuRegistro(u_temp);
    fflush(stdout);
    scanf(" %49s", u_temp.contrasena);

    char *datos[4];
    datos[0] = u_temp.nombre;
    datos[1] = u_temp.apellido;
    datos[2] = u_temp.dni;
    datos[3] = u_temp.contrasena;

    int res = insert_usuario(db, datos);

    if (res == SQLITE_OK) {
        printf("\nRegistro completado con éxito.\n");
        printf("El apartado de consumidor no esta diseñado aún, intentelo más adelante. \n Presione Enter para volver al inicio...");
        fflush(stdout);
        while(getchar() != '\n');
        getchar();
    } else {
        printf("\nError al registrar: El DNI ya existe o hubo un problema con la base de datos.\n");
        printf("Presione Enter para volver al inicio...");
        fflush(stdout);
        while(getchar() != '\n'); // Limpiar buffer
        getchar();                // Pausa
    }
}

void gestionMenuPrincipal(sqlite3 *db, Usuario u_final){
	int opcion = 0;
	int salir = 0;

	while (!salir){
		crearMenuPrincipal();
		fflush(stdout);

		if (scanf(" %d", &opcion) != 1) {
		   while(getchar() != '\n');
		   opcion = 0;
		        }

		switch (opcion) {
			case 1:
				gestionMenuNegocios(db, u_final);
				break;
			case 2:
				gestionMenuUsuario(db, u_final);
				break;
			case 3:
				salir = 1;
				break;
			default:
				printf("Opción invalida \n\n");
		}
	}
}

void gestionMenuNegocios(sqlite3 *db, Usuario u_final){
	int opcion = 0;
	int salir = 0;

	while (!salir){
		crearMenuNegocios();
		fflush(stdout);

		if (scanf(" %d", &opcion) != 1) {
				   while(getchar() != '\n');
				   opcion = 0;
				        }

		switch(opcion){
		case 1:
			gestionarMenuVerNegocio(db);
			break;
		case 2:
			gestionMenuAnyadirNegocios(db);
			break;
		case 3:
			gestionMenuEliminarNegocios(db);
			break;
		case 4:
			gestionMenuModificarNegocios(db);
			break;
		case 5:
			salir = 1;
			break;
		default:
			printf("Opción invalida \n\n");
		}

	}
}

void gestionMenuUsuario(sqlite3 *db, Usuario u_final){

}

void gestionarMenuVerNegocio(sqlite3 *db){
    int t = 0;
    Negocio * negocios = get_negocios(db, &t);
    mostrarNegocios(negocios, t);

    printf("\nPulse Enter para volver.\n");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();

    if (negocios != NULL) {
        free(negocios);
    }
}

void gestionMenuAnyadirNegocios(sqlite3 *db){
    Negocio n;
    memset(&n, 0, sizeof(Negocio));

    crearMenuAnyadirNegocios(n);
    printf("\nIntroduce los datos del nuevo negocio:\n");

    printf("Nombre del negocio: ");
    fflush(stdout);
    scanf(" %74[^\n]", n.nombre);

    printf("Municipio: ");
    fflush(stdout);
    scanf(" %49[^\n]", n.municipio);

    printf("Hora de apertura (ej. 08:00): ");
    fflush(stdout);
    scanf(" %19s", n.hora_apertura);

    printf("Hora de cierre (ej. 20:00): ");
    fflush(stdout);
    scanf(" %19s", n.hora_cierre);

    printf("Tipo de servicio (ej. Taller, Curso...): ");
    fflush(stdout);
    scanf(" %49[^\n]", n.tipo);

    n.fecha = 127;

    int res = insert_negocio(db, n);

    if (res == SQLITE_DONE) {
        printf("\n¡Negocio '%s' añadido con éxito a la base de datos!\n", n.nombre);
    } else {
        printf("\nError: No se pudo añadir (¿Quizás ese nombre ya existe?).\n");
    }

    printf("Presione Enter para volver...");
    fflush(stdout);
    while(getchar() != '\n');
    getchar();
}

void gestionMenuEliminarNegocios(sqlite3 *db){
    char nombre[75];

    Negocio n_vacia;
    memset(&n_vacia, 0, sizeof(Negocio));
    crearMenuEliminarNegocios(n_vacia);
    printf("\nIntroduce el nombre EXACTO del negocio a eliminar: ");
    fflush(stdout);
    scanf(" %74[^\n]", nombre);

    int res = delete_negocio(db, nombre);

    if (res == SQLITE_DONE) {
        printf("\n¡Se ha enviado la orden de eliminar el negocio '%s'!\n", nombre);
    } else {
        printf("\nHubo un problema al intentar eliminar.\n");
    }

    printf("Presione Enter para volver...");
    fflush(stdout);
    while(getchar() != '\n');
    getchar();
}

void gestionMenuModificarNegocios(sqlite3 *db){
    char nombre_actual[75];
    Negocio n_nuevo;
    memset(&n_nuevo, 0, sizeof(Negocio));

    crearMenuModificarNegocios(n_nuevo);

    printf("\nIntroduce el nombre EXACTO del negocio que quieres modificar: ");
    fflush(stdout);
    scanf(" %74[^\n]", nombre_actual);

    printf("\n--- Introduce los NUEVOS datos ---\n");

    printf("Nuevo Municipio: ");
    fflush(stdout);
    scanf(" %49[^\n]", n_nuevo.municipio);

    printf("Nueva Hora de apertura (ej. 08:00): ");
    fflush(stdout);
    scanf(" %19s", n_nuevo.hora_apertura);

    printf("Nueva Hora de cierre (ej. 20:00): ");
    fflush(stdout);
    scanf(" %19s", n_nuevo.hora_cierre);

    printf("Nuevo Tipo de servicio: ");
    fflush(stdout);
    scanf(" %49[^\n]", n_nuevo.tipo);

    int res = update_negocio(db, nombre_actual, n_nuevo);

    if (res == SQLITE_DONE && sqlite3_changes(db) > 0) {
        printf("\n¡Negocio '%s' actualizado con éxito!\n", nombre_actual);
    } else {
        printf("\nNo se pudo actualizar (¿Seguro que el negocio '%s' existe?).\n", nombre_actual);
    }

    printf("Presione Enter para volver...");
    fflush(stdout);
    while(getchar() != '\n');
    getchar();
}
