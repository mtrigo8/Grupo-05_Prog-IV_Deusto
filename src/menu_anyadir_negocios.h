/*
 * menu_anyadir_negocios.h
 */

#ifndef MENU_ANYADIR_NEGOCIOS_H_
#define MENU_ANYADIR_NEGOCIOS_H_

#include "../server/negocio.h"
#include "../server/sqlite3.h"

void crearMenuAnyadirNegocios(Negocio n);
void gestionMenuAnyadirNegocios(sqlite3 *db);

#endif /* MENU_ANYADIR_NEGOCIOS_H_ */
