/*
 * menus.c
 *
 *  Created on: 24 mar 2026
 *      Author: markel.trigo
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>
#include <ctype.h>
#include "menus.h"
#include  "estructuras.h"

int convertirDiasInt(char dias[]){
	//IAG
	int mascara = 0;
	    char buffer[256];

	    strncpy(buffer, dias, sizeof(buffer) - 1);
	    buffer[sizeof(buffer) - 1] = '\0';

	    for (int i = 0; buffer[i] != '\0'; i++) {
	        buffer[i] = toupper((unsigned char)buffer[i]);
	    }

	    char *token = strtok(buffer, ", ");

	    while (token != NULL) {
	        if      (strcmp(token, "LUNES") == 0)     mascara |= 1;
	        else if (strcmp(token, "MARTES") == 0)    mascara |= 2;
	        else if (strcmp(token, "MIERCOLES") == 0) mascara |= 4;
	        else if (strcmp(token, "JUEVES") == 0)    mascara |= 8;
	        else if (strcmp(token, "VIERNES") == 0)   mascara |= 16;
	        else if (strcmp(token, "SABADO") == 0)    mascara |= 32;
	        else if (strcmp(token, "DOMINGO") == 0)   mascara |= 64;

	        token = strtok(NULL, ", ");
	    }

	    return mascara;
}

void convertirIntDias(int mascara, char resultado[]) {
	//IAG
    // Array con los nombres de los días para facilitar la lectura
    const char *nombres_dias[] = {"LUNES", "MARTES", "MIERCOLES", "JUEVES", "VIERNES", "SABADO", "DOMINGO"};

    // Nos aseguramos de que el string de resultado empiece vacío
    resultado[0] = '\0';
    int primero = 1; // Bandera para saber si es el primer día y no poner coma

    // Recorremos los 7 bits (0 al 6)
    for (int i = 0; i < 7; i++) {
        // (1 << i) va generando: 1, 2, 4, 8, 16, 32, 64
        if (mascara & (1 << i)) {
            // Si no es el primer día que añadimos, ponemos una coma antes
            if (!primero) {
                strcat(resultado, ", ");
            }
            // Añadimos el día correspondiente al string
            strcat(resultado, nombres_dias[i]);
            primero = 0; // Ya pasamos el primer elemento
        }
    }

}


void crearMenuBienvenida(){
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

void crearMenuInicioSesion(Usuario u){
	printf("======================= \n");
	printf("Inicio de sesion \n");
	printf("Usuario (DNI): %s \n", u.dni);
	printf("Contraseña: %s \n", u.contrasena);
	printf("======================= \n");
}

void crearMenuRegistro(Usuario u){
	printf("======================= \n");
	printf("Registro \n");
	printf("Nombre: %s \n", u.nombre);
	printf("Apellido: %s \n", u.apellido);
	printf("Usuario (DNI): %s \n", u.dni);
	printf("Contraseña: %s \n", u.contrasena);
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

void crearMenuAnyadirNegocios(Negocio n){
	printf("======================= \n");
	printf("Anyadir negocio\n");
	printf("Nombre del negocio: %s\n", n.nombre);
	printf("Municipio: %s\n", n.municipio);
	printf("Hora apertura(hh:mm) %s\n", n.hora_apertura);
	printf("Hora cierre(hh:mm) %s\n", n.hora_cierre);
	printf("Dias abierto separado por comas %s\n", n.dias);
	printf("Tipo de negocio: %s\n", n.tipo);
	printf("Pulse Enter al finalizar \n");
	printf("======================= \n");
}

void crearMenuEliminarNegocios(Negocio n){
	printf("======================= \n");
	printf("Eliminar negocio \n");
	printf("Negocio: %s\n", n.nombre);
	printf("Municipio: %s\n", n.municipio);
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

void crearMenuModificarNegocios(Negocio n){
	printf("======================= \n");
	printf("Modificar negocio \n");
	printf("Negocio: %s\n", n.nombre);
	printf("Municipio: %s\n",n.municipio);
	printf("Hora apertura(hh:mm) %s\n", n.hora_apertura);
	printf("Hora cierre(hh:mm) %s\n", n.hora_cierre);
	printf("Pulse Enter al finalizar \n");
	printf("======================= \n");
}

void crearMenuUsuario(Usuario u){
	printf("======================= \n");
		printf("Modificacion usuario \n");
		printf("Nombre: %s \n", u.nombre);
		printf("Apellido: %s \n", u.apellido);
		printf("Usuario (DNI): %s \n", u.dni);
		printf("Contraseña: %s \n", u.contrasena);
		printf("======================= \n");
}

void mostrarNegocios(Negocio* negocios, int cantidad_total){
    printf("======================= \n");
    printf("Mostrando todos los negocios en la base de datos\n");
    printf("======================= \n");

    int i = 0;
    while(i < cantidad_total){
        Negocio n = negocios[i];
        char dias[100];
        memset(dias, 0, sizeof(dias));
        convertirIntDias(n.fecha, dias);
        fflush(stdout);
        printf("%d. %s en %s, de %s a %s, abierto: %s, tipo: %s\n",
               i + 1, n.nombre, n.municipio, n.hora_apertura, n.hora_cierre, dias, n.tipo);
        i++;
    }
    printf("======================= \n");
}
