/*
 * menu_negocios.h
 *
 *  Created on: 18 may 2026
 *      Author: markel.trigo
 */

#ifndef MENU_NEGOCIOS_H_
#define MENU_NEGOCIOS_H_

#include "SocketClient.h"
#include "Cache.h"
#include "SocketClient.h"
#include "Sesion.h"
#include "Cache.h"

/* Imprime las opciones del submenu de negocios */
void crearMenuNegocios();

/* Punto de entrada al submenu de negocios desde menu_principal */
void gestionMenuNegocios(SocketClient& sock, CacheOO& cache, const SesionOO& sesion);

#endif /* MENU_NEGOCIOS_H_ */
