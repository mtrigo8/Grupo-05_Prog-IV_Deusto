/*
 * menu_modificar_negocios.h
 */

#ifndef MENU_MODIFICAR_NEGOCIOS_H_
#define MENU_MODIFICAR_NEGOCIOS_H_

#include "negocio.h"
#include "sqlite3.h"

void crearMenuModificarNegocios(char *nombreActual, Negocio n);
void gestionMenuModificarNegocios(sqlite3 *db);

#endif /* MENU_MODIFICAR_NEGOCIOS_H_ */
