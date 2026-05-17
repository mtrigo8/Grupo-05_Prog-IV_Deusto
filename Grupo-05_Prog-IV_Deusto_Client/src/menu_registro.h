#ifndef MENU_REGISTRO_H_
#define MENU_REGISTRO_H_

#include "SocketClient.h"
#include "Sesion.h"

/* Imprime el formulario de registro */
void crearMenuRegistro();

void gestionMenuRegistro(SocketClient& sock, SesionOO& sesion);

#endif /* MENU_REGISTRO_H_ */
