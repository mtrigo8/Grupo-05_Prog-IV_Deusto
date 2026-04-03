#ifndef LOGICAMENUS_H_
#define LOGICAMENUS_H_

#include <stdio.h>
#include <stdlib.h>
#include  "estructuras.h"

void gestionMenuBienvenida(sqlite3 *db);
void gestionMenuInicioSesion(sqlite3 *db);
void gestionMenuRegistro(sqlite3 *db);
void gestionMenuPrincipal(sqlite3 *db, Usuario u_final);
void gestionMenuNegocios(sqlite3 *db, Usuario u_final);
void gestionMenuAnyadirNegocios(sqlite3 *db);
void gestionMenuEliminarNegocios(sqlite3 *db);
void gestionMenuEliminarNegociosConfirm(sqlite3 *db);
void gestionMenuModificarNegocios(sqlite3 *db);
void gestionMenuUsuario(sqlite3 *db, Usuario u_final);
void gestionarMenuVerNegocio(sqlite3 *db);

#endif
