/*
 * menu_modificar_config.h
 */

#ifndef MENU_MODIFICAR_CONFIG_H_
#define MENU_MODIFICAR_CONFIG_H_

#include "../server/config.h"
#include "../server/sqlite3.h"

void crearMenuModificarConfiguracion(Config c);
void gestionMenuModificarConfig(sqlite3 *db, Config *c_actual);

#endif /* MENU_MODIFICAR_CONFIG_H_ */
