/*
 * menus.c
 *
 *  Created on: 24 mar 2026
 *      Author: markel.trigo
 */


#include "menus.h"

void crearMenuBienvenida(){
	printf("======================= \n");
	printf("Bienvenido a Nombre_App \n");
	printf("1. Inicio sesion \n");
	printf("2. Registrarse \n");
	printf("3. Salir \n");
	printf("======================= \n");
}

void crearMenuInicioSesion(){
	printf("======================= \n");
	printf("Inicio de sesion \n");
	printf("Usuario (DNI): \n");
	printf("Contraseña: \n");
	printf("======================= \n");
}

void crearMenuRegistro(){
	printf("======================= \n");
	printf("Registro \n");
	printf("Nombre: \n");
	printf("Apellido: \n");
	printf("Usuario (DNI): \n");
	printf("Contraseña: \n");
	printf("======================= \n");
}


void crearMenuPrincipal(){
	printf("======================= \n");
	printf("1. Gestionar negocios \n");
	printf("2. Gestionar usuario \n");
	printf("3. Salir \n");
	printf("======================= \n");
}

void crearMenuNegocios(){
	printf("======================= \n");
	printf("1. Ver negocios \n");
	printf("2. Anyadir negocios \n");
	printf("3. Eliminar negocios \n");
	printf("4. Modificar negocios \n");
	printf("5. Volver \n");
	printf("======================= \n");
}

void crearMenuAnyadirNegocios(){
	printf("======================= \n");
	printf("Anyadir negocio\n");
	printf("Nombre del negocio: \n");
	printf("Municipio: \n");
	printf("Horario(Separado por coma) \n");
	printf("Pulse Enter al finalizar \n");
	printf("======================= \n");
}

void crearMenuEliminarNegocios(){
	printf("======================= \n");
	printf("Eliminar negocio \n");
	printf("Negocio: \n");
	printf("Municipio: \n");
	printf("Pulse Enter al finalizar \n");
	printf("======================= \n");
}

void crearMenuEliminarNegociosConfirm(char *negocio, char *municipio){
	printf("======================= \n");
	printf("Eliminar negocio \n");
	printf("Seguro que quieros eliminar %s en %s \n", negocio, municipio);
	printf("[s/n]: \n");
	printf("Pulse Enter para confirmar \n");
	printf("======================= \n");
}

void crearMenuModificarNegocios(){
	printf("======================= \n");
	printf("Modificar negocio \n");
	printf("Negocio: \n");
	printf("Municipio: \n");
	printf("Horario(Separado por coma): \n");
	printf("Pulse Enter al finalizar \n");
	printf("======================= \n");
}

void crearMenuUsuario(){
	printf("======================= \n");
	printf("1. Gestionar negocios \n");
	printf("2. Gestionar usuario \n");
	printf("3. Salir \n");
	printf("======================= \n");
}
