#ifndef MENU_PRINCIPAL_H_
#define MENU_PRINCIPAL_H_

#include "SocketClient.h"
#include "Sesion.h"
#include "Cache.h"
#include "menu_reservas.h"

/* Imprime las opciones del menu principal */
void crearMenuPrincipal(const SesionOO& sesion);

void gestionMenuPrincipal(SocketClient& sock,
                          SesionOO&     sesion,
                          CacheOO&      cache);

#endif /* MENU_PRINCIPAL_H_ */
