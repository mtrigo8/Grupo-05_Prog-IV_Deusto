/*
 * menu_anyadir_negocios.h
 */

#ifndef MENU_ANYADIR_NEGOCIOS_H_
#define MENU_ANYADIR_NEGOCIOS_H_

#include "sqlite3.h"
#include "estructuras.h"

void crearMenuAnyadirNegocios(Negocio n);
void gestionMenuAnyadirNegocios(sqlite3 *db);

#endif /* MENU_ANYADIR_NEGOCIOS_H_ */
