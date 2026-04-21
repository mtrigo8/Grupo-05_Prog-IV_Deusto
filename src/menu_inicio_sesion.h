/*
 * menu_inicio_sesion.h
 */

#ifndef MENU_INICIO_SESION_H_
#define MENU_INICIO_SESION_H_

#include "sqlite3.h"
#include "estructuras.h"
#include "config.h"

void crearMenuInicioSesion(Usuario u);
void gestionMenuInicioSesion(sqlite3 *db, Config *c);

#endif /* MENU_INICIO_SESION_H_ */
