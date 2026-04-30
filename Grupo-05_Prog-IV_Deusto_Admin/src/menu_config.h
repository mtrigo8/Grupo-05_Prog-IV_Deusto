/*
 * menu_config.h
 */

#ifndef MENU_CONFIG_H_
#define MENU_CONFIG_H_

#include "config.h"
#include "sqlite3.h"

void crearMenuConfig();
void gestionMenuConfig(sqlite3 *db, Config *c);

#endif /* MENU_CONFIG_H_ */
