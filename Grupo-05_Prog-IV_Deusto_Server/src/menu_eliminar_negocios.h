/*
 * menu_eliminar_negocios.h
 */

#ifndef MENU_ELIMINAR_NEGOCIOS_H_
#define MENU_ELIMINAR_NEGOCIOS_H_

#include "negocio.h"
#include "sqlite3.h"

void crearMenuEliminarNegocios(Negocio n);
void crearMenuEliminarNegociosConfirm(Negocio n);
void gestionMenuEliminarNegocios(sqlite3 *db);

#endif /* MENU_ELIMINAR_NEGOCIOS_H_ */
