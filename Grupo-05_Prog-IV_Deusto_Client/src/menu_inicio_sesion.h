#ifndef MENU_INICIO_SESION_H_
#define MENU_INICIO_SESION_H_

#include "SocketClient.h"
#include "Sesion.h"
#include "Cache.h"

/* Imprime el formulario de inicio de sesion */
void crearMenuInicioSesion();

void gestionMenuInicioSesion(SocketClient& sock,
                              SesionOO&     sesion,
                              CacheOO&      cache);

#endif /* MENU_INICIO_SESION_H_ */
