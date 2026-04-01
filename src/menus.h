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

#endif /* MENUS_H_ */
