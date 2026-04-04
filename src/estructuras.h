/*
 * estructuras.h
 *
 *  Created on: 1 abr 2026
 *      Author: markel.trigo
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

typedef struct {
    char nombre[50];
    char apellido[50];
    char dni[20];
    char contrasena[50];
    int rol; //1 = admin, 2 = usuario
} Usuario;

typedef struct {
    char nombre[75];
    char municipio[50];
    char hora_apertura[20];
    char hora_cierre[20];
    int fecha;//dias en binario
    char tipo[50];
    char dias[50];//dias en string, provisional
    char descripcion[1000];
} Negocio;

enum Dias {
    LUNES     = 1 << 0, // 1
    MARTES    = 1 << 1, // 2
    MIERCOLES = 1 << 2, // 4
    JUEVES    = 1 << 3, // 8
    VIERNES   = 1 << 4, // 16
    SABADO    = 1 << 5, // 32
    DOMINGO   = 1 << 6  // 64
};

#endif /* ESTRUCTURAS_H_ */
