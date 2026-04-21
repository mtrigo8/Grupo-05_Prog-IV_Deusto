/*
 * menu_ver_negocios.h
 */

#ifndef MENU_VER_NEGOCIOS_H_
#define MENU_VER_NEGOCIOS_H_

#include "sqlite3.h"
#include "estructuras.h"

void mostrarNegocios(Negocio *negocios, int cantidad_total);
void gestionMenuVerNegocios(sqlite3 *db);

#endif /* MENU_VER_NEGOCIOS_H_ */
