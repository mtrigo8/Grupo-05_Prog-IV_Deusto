#include "menu_inicio_sesion.h"
#include "Protocol.h"
#include "menu_principal.h"


/* Declaracion extern "C": sha256_hex esta en hash.c (compilado como C) */
extern "C"
{
    #include "hash.h"
}

#include <iostream>
#include <string>
#include <limits>

/* ── Vista ───────────────────────────────────────────────────────────────── */

void crearMenuInicioSesion()
{
    std::cout << "\n=======================\n"
              << "    Inicio de sesion\n"
              << "=======================\n";
}

/* ── Logica ──────────────────────────────────────────────────────────────── */

void gestionMenuInicioSesion(SocketClient& sock,
                              SesionOO&     sesion,
                              CacheOO&      cache)
{
    /* Suprimir advertencia de cache no usada en este menu */
    (void)cache;

    crearMenuInicioSesion();

    /* ── 1. Pedir DNI ──────────────────────────────────────────────────── */

    std::string dni;
    std::cout << "DNI: ";
    std::getline(std::cin, dni);

    if (dni.empty())
    {
        std::cout << "El DNI no puede estar vacio.\n";
        return;
    }

    /* ── 2. Pedir contrasena ───────────────────────────────────────────── */

    std::string contrasena;
    std::cout << "Contrasena: ";
    std::getline(std::cin, contrasena);

    if (contrasena.empty())
    {
        std::cout << "La contrasena no puede estar vacia.\n";
        return;
    }

    /* ── 3. Hashear contrasena con SHA-256 ─────────────────────────────── */

    char hashContrasena[65] = {0};
    sha256_hex(contrasena.c_str(), hashContrasena);

    /* ── 4. Enviar comando LOGIN al servidor ───────────────────────────── */

    if (!sock.enviar(CMD_LOGIN))
    {
        std::cout << "Error: no se pudo enviar el comando al servidor.\n";
        return;
    }

    /* ── 5. Enviar parametros: dni|hash ────────────────────────────────── */

    std::string params = buildLogin(dni, std::string(hashContrasena));

    if (!sock.enviar(params))
    {
        std::cout << "Error: no se pudieron enviar los datos al servidor.\n";
        return;
    }

    /* ── 6. Recibir y parsear respuesta ────────────────────────────────── */

    std::string respuesta = sock.recibir();

    if (respuesta.empty())
    {
        std::cout << "Error: no se recibio respuesta del servidor.\n";
        return;
    }

    ParsedAuth auth = parseLogin(respuesta);

    /* ── 7. Evaluar resultado ──────────────────────────────────────────── */

    if (auth.ok)
    {
        sesion.iniciarSesion(auth.idUsuario, auth.nombre, auth.rol);

        std::cout << "\nBienvenido, " << sesion.getNombre() << "!\n";

        gestionMenuPrincipal(sock, sesion, cache);
    }
    else
    {

        if (auth.errorMsg == "CREDENCIALES_INVALIDAS")
        {
            std::cout << "\nError: DNI o contrasena incorrectos.\n";
        }
        else
        {
            std::cout << "\nError al iniciar sesion: " << auth.errorMsg << "\n";
        }
    }
}
