#include "menu_principal.h"
#include "Protocol.h"
#include "menu_negocios.h" // Incluimos el nuevo menu de negocios
#include "menu_reservas.h"

#include <iostream>
#include <limits>
#include <vector>
#include <string>

/* ── Vista ───────────────────────────────────────────────────────────────── */

void crearMenuPrincipal(const SesionOO& sesion)
{
    std::cout << "\n=======================\n"
              << "  Hola, " << sesion.getNombre() << "\n"
              << "=======================\n"
              << "1. Ver negocios\n"
              << "2. Mis reservas\n"
              << "3. Cerrar sesion\n"
              << "=======================\n";
}

/* ── Helpers internos ────────────────────────────────────────────────────── */

static bool gestionCerrarSesion(SocketClient& sock, SesionOO& sesion,
                                 CacheOO& cache)
{
    if (!sock.enviar(CMD_LOGOUT))
    {
        std::cout << "Error: no se pudo enviar el comando al servidor.\n";
        return false;
    }

    if (!sock.enviar(buildLogout(sesion.getId())))
    {
        std::cout << "Error: no se pudieron enviar los parametros.\n";
        return false;
    }

    std::string respuesta = sock.recibir();

    if (esOk(respuesta))
    {
        std::cout << "\nSesion cerrada correctamente. Hasta pronto, "
                  << sesion.getNombre() << ".\n\n";

        /* Limpiar estado local */
        cache.limpiarTodo();
        sesion.cerrarSesion();

        return true;
    }
    else
    {
        std::cout << "Error al cerrar sesion: " << respuesta << "\n";
        return false;
    }
}

/* ── Logica principal ────────────────────────────────────────────────────── */

void gestionMenuPrincipal(SocketClient& sock,
                          SesionOO&     sesion,
                          CacheOO&      cache)
{
    int  opcion = 0;
    bool salir  = false;

    while (!salir && sesion.estaAutenticado())
    {
        crearMenuPrincipal(sesion);

        if (!(std::cin >> opcion))
        {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            opcion = 0;
        }
        else
        {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        switch (opcion)
        {
            case 1:
                /* Delegar al nuevo submenu de negocios */
                gestionMenuNegocios(sock, cache, sesion);
                break;

            case 2:
                /* Delegar al submenu de reservas */
                gestionMenuReservas(sock, cache, sesion);
                break;

            case 3:
                salir = gestionCerrarSesion(sock, sesion, cache);
                break;

            default:
                std::cout << "Opcion invalida.\n";
                break;
        }
    }
}
