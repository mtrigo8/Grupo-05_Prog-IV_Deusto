#ifndef MENU_RESERVAS_H_
#define MENU_RESERVAS_H_

#include "SocketClient.h"
#include "Sesion.h"
#include "Cache.h"

/* Imprime las opciones del submenu de reservas */
void crearMenuReservas();

/* Punto de entrada al submenu de reservas desde menu_principal */
void gestionMenuReservas(SocketClient&   sock,
                         CacheOO&        cache,
                         const SesionOO& sesion);

#endif /* MENU_RESERVAS_H_ */
