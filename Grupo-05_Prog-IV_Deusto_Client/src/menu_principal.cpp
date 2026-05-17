
#include "menu_principal.h"
#include "Protocol.h"
#include "NegocioFactory.h"
#include "Reserva.h"

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


static void cargarNegocios(SocketClient& sock, CacheOO& cache)
{
    cache.limpiarNegocios();

    if (!sock.enviar(CMD_GET_SERVICIOS))
    {
        std::cout << "Error: no se pudo enviar el comando al servidor.\n";
        return;
    }

    if (!sock.enviar(buildGetServicios()))
    {
        std::cout << "Error: no se pudieron enviar los parametros.\n";
        return;
    }

    std::vector<std::string> lineas = recibirLista(sock);

    /* Comprobar si el servidor devolvio un error en lugar de la lista */
    if (lineas.size() == 1 && esError(lineas[0]))
    {
        std::cout << "Error al obtener negocios: " << lineas[0] << "\n";
        return;
    }

    std::vector<ParsedServicio> parsed = parseListaServicios(lineas);

    for (int i = 0; i < (int)parsed.size(); i++)
    {
        NegocioOO* obj = NegocioFactory::crear(parsed[i]);
        cache.agregarNegocio(obj);
    }

    std::cout << cache.getTotalNegocios() << " negocios cargados.\n";
}


static void cargarReservas(SocketClient& sock, CacheOO& cache,
                           const SesionOO& sesion)
{
    cache.limpiarReservas();

    if (!sock.enviar(CMD_GET_RESERVA))
    {
        std::cout << "Error: no se pudo enviar el comando al servidor.\n";
        return;
    }

    if (!sock.enviar(buildGetReserva(sesion.getId())))
    {
        std::cout << "Error: no se pudieron enviar los parametros.\n";
        return;
    }

    std::vector<std::string> lineas = recibirLista(sock);

    if (lineas.size() == 1 && esError(lineas[0]))
    {
        std::cout << "Error al obtener reservas: " << lineas[0] << "\n";
        return;
    }

    std::vector<ParsedReserva> parsed = parseListaReservas(lineas);

    for (int i = 0; i < (int)parsed.size(); i++)
    {
        Reserva* r = new Reserva(
            parsed[i].idReserva,
            sesion.getId(),
            parsed[i].idServicio,
            parsed[i].fecha,
            parsed[i].nombreServicio,
            parsed[i].estado
        );
        cache.agregarReserva(r);
    }

    std::cout << cache.getTotalReservas() << " reservas cargadas.\n";
}

static void gestionVerNegocios(SocketClient& sock, CacheOO& cache)
{
    std::cout << "\n--- Ver negocios ---\n";

    /* Solo recargar si la cache esta vacia */
    if (cache.getTotalNegocios() == 0)
    {
        std::cout << "Cargando negocios del servidor...\n";
        cargarNegocios(sock, cache);
    }

    if (cache.getTotalNegocios() == 0)
    {
        std::cout << "No hay negocios disponibles.\n";
        return;
    }

    cache.mostrarNegocios();

    std::cout << "\nPulsa Enter para volver...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


static void gestionMisReservas(SocketClient& sock, CacheOO& cache,
                               const SesionOO& sesion)
{
    std::cout << "\n--- Mis reservas ---\n"
              << "Cargando reservas del servidor...\n";

    cargarReservas(sock, cache, sesion);

    if (cache.getTotalReservas() == 0)
    {
        std::cout << "No tienes reservas activas.\n";
    }
    else
    {
        cache.mostrarReservas();
    }

    std::cout << "\nPulsa Enter para volver...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


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
                gestionVerNegocios(sock, cache);
                break;

            case 2:
                gestionMisReservas(sock, cache, sesion);
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
