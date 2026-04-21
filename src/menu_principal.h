/*
 * menu_principal.h
 */

#ifndef MENU_PRINCIPAL_H_
#define MENU_PRINCIPAL_H_

#include "sqlite3.h"
#include "estructuras.h"
#include "config.h"

void crearMenuPrincipal();
void gestionMenuPrincipal(sqlite3 *db, Usuario u_final, Config *c);

#endif /* MENU_PRINCIPAL_H_ */
