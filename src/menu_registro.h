/*
 * menu_registro.h
 */

#ifndef MENU_REGISTRO_H_
#define MENU_REGISTRO_H_

#include "../server/usuario.h"
#include "../server/sqlite3.h"

void crearMenuRegistro(Usuario u);
void gestionMenuRegistro(sqlite3 *db);

#endif /* MENU_REGISTRO_H_ */
