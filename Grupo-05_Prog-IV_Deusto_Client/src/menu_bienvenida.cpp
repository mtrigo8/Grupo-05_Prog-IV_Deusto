#include "menu_bienvenida.h"

#include <iostream>
#include <limits>

/* Descomentar cuando los menus esten implementados */
/* #include "menu_inicio_sesion.h" */
/* #include "menu_registro.h"      */

/* ── Vista ────────────────────────────────────────────────────────────── */

void crearMenuBienvenida()
{
    std::cout << "=======================\n"
              << "  ____ _ _         _   _       _      \n"
              << " / ___(_) |_ _   _| | | |_   _| |__   \n"
              << "| |   | | __| | | | |_| | | | | '_ \\ \n"
              << "| |___| | |_| |_| |  _  | |_| | |_) | \n"
              << " \\____|_|\\__|\\__, |_| |_|\\__,_|_.__/  \n"
              << "             |___/                    \n"
              << "=======================\n"
              << "Bienvenido a CityHub\n"
              << "1. Inicio sesion\n"
              << "2. Registrarse\n"
              << "3. Salir\n"
              << "=======================\n";
}

/* ── Logica ───────────────────────────────────────────────────────────── */

void gestionMenuBienvenida(SocketClient& sock,
                           SesionOO&     sesion,
                           CacheOO&      cache)
{
    int  opcion = 0;
    bool salir  = false;

    while (!salir)
    {
        crearMenuBienvenida();

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
                /* gestionMenuInicioSesion(sock, sesion, cache); */
                std::cout << "[ Inicio de sesion — proximamente ]\n\n";
                break;

            case 2:
                /* gestionMenuRegistro(sock, sesion); */
                std::cout << "[ Registro — proximamente ]\n\n";
                break;

            case 3:
                salir = true;
                break;

            default:
                std::cout << "Opcion invalida\n\n";
                break;
        }
    }
}
