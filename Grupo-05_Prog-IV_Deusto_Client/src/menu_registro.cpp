#include "menu_registro.h"
#include "Protocol.h"

extern "C"
{
    #include "hash.h"
}

#include <iostream>
#include <string>
#include <limits>

/* ── Helpers internos ────────────────────────────────────────────────────── */


static std::string pedirCampo(const std::string& etiqueta)
{
    std::string valor;

    while (true)
    {
        std::cout << etiqueta << ": ";
        std::getline(std::cin, valor);

        if (!valor.empty())
        {
            break;
        }

        std::cout << "  El campo no puede estar vacio. Intentalo de nuevo.\n";
    }

    return valor;
}

/* ── Vista ───────────────────────────────────────────────────────────────── */

void crearMenuRegistro()
{
    std::cout << "\n=======================\n"
              << "       Registro\n"
              << "=======================\n"
              << "Rellena los siguientes campos:\n\n";
}

/* ── Logica ──────────────────────────────────────────────────────────────── */

void gestionMenuRegistro(SocketClient& sock, SesionOO& sesion)
{
    (void)sesion;

    bool reintentar = true;

    while (reintentar)
    {
        crearMenuRegistro();

        /* ── 1. Recoger datos del usuario ──────────────────────────────── */

        std::string nombre    = pedirCampo("Nombre");
        std::string apellido  = pedirCampo("Apellido");
        std::string dni       = pedirCampo("DNI");
        std::string password  = pedirCampo("Contrasena");

        /* ── 2. Hashear contrasena ─────────────────────────────────────── */

        char hashPassword[65] = {0};
        sha256_hex(password.c_str(), hashPassword);

        /* ── 3. Enviar comando REGISTER ────────────────────────────────── */

        if (!sock.enviar(CMD_REGISTER))
        {
            std::cout << "Error: no se pudo enviar el comando al servidor.\n";
            return;
        }

        /* ── 4. Enviar parametros: nombre|apellido|dni|hash ────────────── */

        std::string params = buildRegister(nombre, apellido, dni,
                                           std::string(hashPassword));

        if (!sock.enviar(params))
        {
            std::cout << "Error: no se pudieron enviar los datos al servidor.\n";
            return;
        }

        /* ── 5. Recibir y parsear respuesta ────────────────────────────── */

        std::string respuesta = sock.recibir();

        if (respuesta.empty())
        {
            std::cout << "Error: no se recibio respuesta del servidor.\n";
            return;
        }

        ParsedAuth auth = parseRegister(respuesta);

        /* ── 6. Evaluar resultado ──────────────────────────────────────── */

        if (auth.ok)
        {
            /* Registro exitoso */
            std::cout << "\nRegistro completado correctamente.\n"
                      << "Tu identificador de usuario es: " << auth.idUsuario << "\n"
                      << "Ya puedes iniciar sesion con tu DNI y contrasena.\n\n";

            reintentar = false;
        }
        else if (auth.errorMsg == "DNI_DUPLICADO")
        {
            /*
             * DNI ya registrado: informar y ofrecer reintentar con otro DNI
             * o volver al menu de bienvenida.
             */
            std::cout << "\nError: el DNI " << dni
                      << " ya esta registrado en el sistema.\n"
                      << "Si ya tienes cuenta, usa la opcion de inicio de sesion.\n\n";

            std::cout << "Opciones:\n"
                      << "  1. Intentar con otro DNI\n"
                      << "  2. Volver al menu anterior\n"
                      << "Opcion: ";

            int opcion = 0;
            if (!(std::cin >> opcion))
            {
                std::cin.clear();
                opcion = 2;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (opcion == 1)
            {
                reintentar = true;
            }
            else
            {
                reintentar = false;
            }
        }
        else
        {
            /* Error generico */
            std::cout << "\nError al registrarse: " << auth.errorMsg << "\n"
                      << "Intentalo de nuevo mas tarde.\n\n";

            reintentar = false;
        }
    }
}
