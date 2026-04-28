/*
 * menu_principal.h
 */

#ifndef MENU_PRINCIPAL_H_
#define MENU_PRINCIPAL_H_

#include "../server/config.h"
#include "../server/usuario.h"
#include "../server/sqlite3.h"

void crearMenuPrincipal(void);
void gestionMenuPrincipal(sqlite3 *db, Usuario u_final, Config *c);

#endif /* MENU_PRINCIPAL_H_ */
