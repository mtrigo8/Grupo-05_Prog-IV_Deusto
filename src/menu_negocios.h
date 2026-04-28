/*
 * menu_negocios.h
 */

#ifndef MENU_NEGOCIOS_H_
#define MENU_NEGOCIOS_H_

#include "usuario.h"
#include "sqlite3.h"

void crearMenuNegocios(void);
void gestionMenuNegocios(sqlite3 *db, Usuario u_final);

#endif /* MENU_NEGOCIOS_H_ */
