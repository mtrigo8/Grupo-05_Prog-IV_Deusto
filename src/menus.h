/*
 * menus.h
 *
 *  Created on: 24 mar 2026
 *      Author: markel.trigo
 */

#ifndef MENUS_H_
#define MENUS_H_

#include <stdio.h>
#include <stdlib.h>
#include  "estructuras.h"

int convertirDiasInt(char dias[]);
void convertirIntDias(int mascara, char resultado[]);
void crearMenuBienvenida();
void crearMenuInicioSesion(Usuario u);
void crearMenuRegistro(Usuario u);
void crearMenuPrincipal();
void crearMenuNegocios();
void crearMenuAnyadirNegocios(Negocio n);
void crearMenuEliminarNegocios(Negocio n);
void crearMenuEliminarNegociosConfirm(char *negocio, char *municipio);
void crearMenuModificarNegocios(Negocio n);
void crearMenuUsuario(Usuario u);
void mostrarNegocios(Negocio* negocios, int cantidad_total);



#endif /* MENUS_H_ */
