/*
 * menu_negocios.h
 */

#ifndef MENU_NEGOCIOS_H_
#define MENU_NEGOCIOS_H_

#include "sqlite3.h"
#include "estructuras.h"

void crearMenuNegocios();
void gestionMenuNegocios(sqlite3 *db, Usuario u_final);

#endif /* MENU_NEGOCIOS_H_ */
