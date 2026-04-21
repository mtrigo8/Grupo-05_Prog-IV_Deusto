/*
 * menu_bienvenida.h
 */

#ifndef MENU_BIENVENIDA_H_
#define MENU_BIENVENIDA_H_

#include "sqlite3.h"
#include "config.h"

void crearMenuBienvenida();
void gestionMenuBienvenida(sqlite3 *db, Config *c);

#endif /* MENU_BIENVENIDA_H_ */
