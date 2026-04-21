/*
 * estructuras.c
 *
 *  Created on: 21 abr 2026
 *      Author: markel.trigo
 *
 * Implementacion de las utilidades de conversion de dias:
 *   - convertirDiasInt  : string "LUNES, MARTES, ..." -> mascara de bits
 *   - convertirIntDias  : mascara de bits             -> string "LUNES, MARTES, ..."
 */

#include <string.h>
#include <ctype.h>
#include "estructuras.h"

/*
 * Convierte una cadena de dias separados por comas (e.g. "Lunes, Martes")
 * a una mascara de bits usando el enum Dias.
 * No distingue mayusculas/minusculas.
 * Devuelve la mascara resultante (0 si no se reconoce ningun dia).
 */
int convertirDiasInt(char dias[]) {
    int mascara = 0;
    char buffer[256];

    strncpy(buffer, dias, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    /* Pasar a mayusculas para comparacion insensible a mayusculas */
    for (int i = 0; buffer[i] != '\0'; i++) {
        buffer[i] = toupper((unsigned char)buffer[i]);
    }

    char *token = strtok(buffer, ", ");
    while (token != NULL) {
        if      (strcmp(token, "LUNES")     == 0) mascara |= LUNES;
        else if (strcmp(token, "MARTES")    == 0) mascara |= MARTES;
        else if (strcmp(token, "MIERCOLES") == 0) mascara |= MIERCOLES;
        else if (strcmp(token, "JUEVES")    == 0) mascara |= JUEVES;
        else if (strcmp(token, "VIERNES")   == 0) mascara |= VIERNES;
        else if (strcmp(token, "SABADO")    == 0) mascara |= SABADO;
        else if (strcmp(token, "DOMINGO")   == 0) mascara |= DOMINGO;

        token = strtok(NULL, ", ");
    }

    return mascara;
}

/*
 * Convierte una mascara de bits a una cadena de dias separados por comas.
 * 'resultado' debe tener espacio suficiente (al menos 70 bytes).
 */
void convertirIntDias(int mascara, char resultado[]) {
    const char *nombres_dias[] = {
        "LUNES", "MARTES", "MIERCOLES",
        "JUEVES", "VIERNES", "SABADO", "DOMINGO"
    };

    resultado[0] = '\0';
    int primero = 1;

    for (int i = 0; i < 7; i++) {
        if (mascara & (1 << i)) {
            if (!primero) strcat(resultado, ", ");
            strcat(resultado, nombres_dias[i]);
            primero = 0;
        }
    }
}
