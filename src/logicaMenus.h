#ifndef LOGICAMENUS_H_
#define LOGICAMENUS_H_

#include <stdio.h>
#include <stdlib.h>
#include  "estructuras.h"
#include "config.h"

void gestionMenuBienvenida(sqlite3 *db, Config *c);
void gestionMenuInicioSesion(sqlite3 *db, Config *c);
void gestionMenuRegistro(sqlite3 *db);
void gestionMenuPrincipal(sqlite3 *db, Usuario u_final, Config *c);
void gestionMenuNegocios(sqlite3 *db, Usuario u_final);
void gestionMenuAnyadirNegocios(sqlite3 *db);
void gestionMenuEliminarNegocios(sqlite3 *db);
void gestionMenuEliminarNegociosConfirm(sqlite3 *db);
void gestionMenuModificarNegocios(sqlite3 *db);
void gestionarMenuVerNegocio(sqlite3 *db);
void gestionMenuConfig(sqlite3 *db, Config *c);
void gestionMenuModificarConfig(sqlite3 *db, Config *c);

#endif
