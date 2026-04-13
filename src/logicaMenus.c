#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "estructuras.h"
#include "menus.h"
#include "db.h"
#include "logicaMenus.h"
#include "config.h"

void gestionMenuBienvenida(sqlite3 *db, Config *c){
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
				gestionMenuInicioSesion(db, c);
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

void gestionMenuInicioSesion(sqlite3 *db, Config *c) {
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
    		gestionMenuPrincipal(db, u_final, c);
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

void gestionMenuPrincipal(sqlite3 *db, Usuario u_final, Config *c){
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
				gestionMenuConfig(db, c);
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

    fflush(stdout);
    scanf(" %74[^\n]", n.nombre);

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    scanf(" %49[^\n]", n.municipio);

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    scanf(" %19s", n.hora_apertura);

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    scanf(" %19s", n.hora_cierre);

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    scanf(" %49[^\n]", n.tipo);

    char dias_temp[256];
    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    scanf(" %255[^\n]", dias_temp);
    n.fecha = convertirDiasInt(dias_temp);

    int res = insert_negocio(db, n);

    if (res == SQLITE_DONE) {
        printf("\n¡Negocio '%s' añadido con éxito a la base de datos!\n", n.nombre);
    } else {
        printf("\nError: No se pudo añadir (¿Quizás ese nombre ya existe?).\n");
    }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    while(getchar() != '\n');
    getchar();
}

void gestionMenuEliminarNegocios(sqlite3 *db){
    char confirmacion;

    Negocio n_vacia;
    memset(&n_vacia, 0, sizeof(Negocio));
    crearMenuEliminarNegocios(n_vacia);

    fflush(stdout);
    scanf(" %74[^\n]", n_vacia.nombre);
    crearMenuEliminarNegocios(n_vacia);
    fflush(stdout);
    scanf(" %49[^\n]", n_vacia.municipio);


    while(getchar() != '\n');
    crearMenuEliminarNegociosConfirm(n_vacia);

    scanf(" %c", &confirmacion);
    while(getchar() != '\n');

    if (confirmacion == 's' || confirmacion == 'S') {
    	int res = delete_negocio(db, n_vacia.nombre);

    	if (res == SQLITE_DONE) {
    		if (sqlite3_changes(db) > 0) {
    			printf("\n¡Se ha eliminado el negocio '%s'!\n", n_vacia.nombre);
    		} else {
    			printf("\nNo se encontro ningun negocio llamado '%s'.\n", n_vacia.nombre);
    		}
    	} else {
    		printf("\nHubo un problema al intentar eliminar en la base de datos.\n");
    	}
    } else {
    	printf("\nOperacion cancelada. El negocio no ha sido borrado.\n");
    }


    printf("Presione Enter para volver...");
    fflush(stdout);
    getchar();
}

void gestionMenuModificarNegocios(sqlite3 *db){
    char nombre_actual[75];
    Negocio n_nuevo;
    memset(&n_nuevo, 0, sizeof(Negocio));

    crearMenuModificarNegocios(nombre_actual, n_nuevo);

    fflush(stdout);
    scanf(" %74[^\n]", nombre_actual);
    while(getchar() != '\n');

    crearMenuModificarNegocios(nombre_actual, n_nuevo);

    fflush(stdout);
    scanf(" %74[^\n]", n_nuevo.nombre);
    fflush(stdout);
    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    scanf(" %49[^\n]", n_nuevo.municipio);
    while(getchar() != '\n');

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    scanf(" %19s", n_nuevo.hora_apertura);

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    scanf(" %19s", n_nuevo.hora_cierre);
    while(getchar() != '\n');

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    scanf(" %49[^\n]", n_nuevo.tipo);
    while(getchar() != '\n');

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    scanf(" %255[^\n]", n_nuevo.dias);
    while(getchar() != '\n');
    n_nuevo.fecha = convertirDiasInt(n_nuevo.dias);


    int res = update_negocio(db, nombre_actual, n_nuevo);

    if (res == SQLITE_DONE && sqlite3_changes(db) > 0) {
        printf("\n¡Negocio '%s' actualizado con éxito!\n", nombre_actual);
    } else {
        printf("\nNo se pudo actualizar (¿Seguro que el negocio '%s' existe?).\n", nombre_actual);
    }

    fflush(stdout);
    getchar();
}

void gestionMenuConfig(sqlite3 *db, Config *c){

	int opcion = 0;
	int salir = 0;

	while (!salir){
		crearMenuConfig();
		fflush(stdout);

		if (scanf(" %d", &opcion) != 1) {
		   while(getchar() != '\n');
		   opcion = 0;
		        }

		switch (opcion) {
			case 1:
				config_mostrar(c);
				printf("Presione Enter para volver...\n");
				fflush(stdout);
				getchar();
				getchar();
				break;
			case 2:
				gestionMenuModificarConfig(db, c);
				break;
			case 3:
				salir = 1;
				break;
			default:
				printf("Opción invalida \n\n");
			}
	}
}
void gestionMenuModificarConfig(sqlite3 *db, Config *c_actual) {
	Config c_nueva;
	// Inicializamos la nueva config con los valores actuales por si el usuario no cambia alguno
	memcpy(&c_nueva, c_actual, sizeof(Config));

		// 1. Pedir DB Path
	crearMenuModificarConfiguracion(c_nueva);
	printf("Nuevo DB path: ");
	fflush(stdout);
	scanf(" %255[^\n]", c_nueva.db_path); // Ajusta el tamaño según tu struct
	while (getchar() != '\n');

		// 2. Pedir Admin User (DNI)
	crearMenuModificarConfiguracion(c_nueva);
	printf("Nuevo Admin user (DNI): ");
	fflush(stdout);
	scanf(" %19s", c_nueva.admin_dni);
	while (getchar() != '\n');

		// 3. Pedir Admin Password
	crearMenuModificarConfiguracion(c_nueva);
	printf("Nueva Admin password: ");
	fflush(stdout);
	scanf(" %49s", c_nueva.admin_password);
	while (getchar() != '\n');

		// 4. Pedir Log Path
	crearMenuModificarConfiguracion(c_nueva);
	printf("Nuevo Log path: ");
	fflush(stdout);
	scanf(" %255[^\n]", c_nueva.log_path);
	while (getchar() != '\n');
		// 5. Pedir Número máximo de negocios
	crearMenuModificarConfiguracion(c_nueva);
	printf("Nuevo numero maximo de negocios: ");
	fflush(stdout);
	if (scanf("%d", &c_nueva.max_negocios) != 1) {
		c_nueva.max_negocios = c_actual->max_negocios; // Backup si falla el input
	}
	while (getchar() != '\n');

		// Pantalla final antes de guardar
	crearMenuModificarConfiguracion(c_nueva);
	printf("\nGuardando cambios...\n");

		// Supongo que tienes una función de actualización en base de datos
		// Si no necesitas guardarlo en DB y es solo en memoria, omite esta parte
	config_guardar(&c_nueva);


	printf("Pulse Enter para continuar... \n");
	fflush(stdout);
	getchar();
}
