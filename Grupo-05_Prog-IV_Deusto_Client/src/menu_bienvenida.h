

#ifndef MENU_BIENVENIDA_H_
#define MENU_BIENVENIDA_H_

#include "SocketClient.h"
#include "Sesion.h"
#include "Cache.h"

/* Imprime el banner y las opciones del menu de bienvenida */
void crearMenuBienvenida();


void gestionMenuBienvenida(SocketClient& sock,
                           SesionOO&     sesion,
                           CacheOO&      cache);

#endif /* MENU_BIENVENIDA_H_ */
