/*
 * menu_config.h
 */

#ifndef MENU_CONFIG_H_
#define MENU_CONFIG_H_

#include "sqlite3.h"
#include "config.h"

void crearMenuConfig();
void gestionMenuConfig(sqlite3 *db, Config *c);

#endif /* MENU_CONFIG_H_ */
