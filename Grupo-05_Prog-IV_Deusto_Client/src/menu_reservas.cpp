#include "menu_reservas.h"
#include "Protocol.h"
#include "Reserva.h"

#include <iostream>
#include <limits>
#include <vector>
#include <string>

/* ── Vista ───────────────────────────────────────────────────────────────── */

void crearMenuReservas()
{
    std::cout << "\n=======================\n"
              << "      Mis reservas\n"
              << "=======================\n"
              << "1. Ver mis reservas\n"
              << "2. Cancelar una reserva\n"
              << "3. Volver\n"
              << "=======================\n";
}

/* ── Helpers internos ────────────────────────────────────────────────────── */

static void cargarReservas(SocketClient&   sock,
                           CacheOO&        cache,
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

static void gestionVerReservas(SocketClient&   sock,
                               CacheOO&        cache,
                               const SesionOO& sesion)
{
    std::cout << "\nCargando reservas del servidor...\n";
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

static void gestionCancelarReserva(SocketClient&   sock,
                                   CacheOO&        cache,
                                   const SesionOO& sesion)
{
    /* Recargar siempre para tener datos frescos antes de cancelar */
    std::cout << "\nCargando reservas del servidor...\n";
    cargarReservas(sock, cache, sesion);

    if (cache.getTotalReservas() == 0)
    {
        std::cout << "No tienes reservas que cancelar.\n";
        std::cout << "\nPulsa Enter para volver...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    cache.mostrarReservas();

    /* Pedir ID de la reserva a cancelar */
    std::cout << "\nIntroduce el numero de reserva a cancelar (0 para volver): ";
    int idReserva = 0;

    if (!(std::cin >> idReserva))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada invalida.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (idReserva == 0)
    {
        return;
    }

    /* Verificar que la reserva existe en cache y pertenece al usuario */
    Reserva* reserva = cache.buscarReservaPorId(idReserva);

    if (reserva == nullptr)
    {
        std::cout << "No se encontro la reserva #" << idReserva << ".\n";
        return;
    }

    if (!reserva->estaActiva())
    {
        std::cout << "La reserva #" << idReserva
                  << " no esta activa y no puede cancelarse.\n";
        return;
    }

    /* Enviar CMD_CANCEL_RESERVA */
    if (!sock.enviar(CMD_CANCEL_RESERVA))
    {
        std::cout << "Error: no se pudo enviar el comando al servidor.\n";
        return;
    }

    if (!sock.enviar(buildCancelReserva(idReserva, sesion.getId())))
    {
        std::cout << "Error: no se pudieron enviar los parametros.\n";
        return;
    }

    std::string respuesta = sock.recibir();

    if (esOk(respuesta))
    {
        std::cout << "Reserva #" << idReserva << " cancelada correctamente.\n";

        /* Forzar recarga la proxima vez */
        cache.limpiarReservas();
    }
    else if (esError(respuesta))
    {
        std::vector<std::string> campos = splitSEP(respuesta);
        std::string motivo = (campos.size() >= 2) ? campos[1] : respuesta;

        if (motivo == "NO_CANCELABLE")
        {
            std::cout << "Error: la reserva no puede cancelarse en este momento.\n";
        }
        else if (motivo == "NO_AUTORIZADO")
        {
            std::cout << "Error: no tienes permiso para cancelar esta reserva.\n";
        }
        else if (motivo == "NO_ENCONTRADO")
        {
            std::cout << "Error: la reserva no existe en el servidor.\n";
        }
        else
        {
            std::cout << "Error al cancelar la reserva: " << motivo << "\n";
        }
    }
    else
    {
        std::cout << "Respuesta inesperada del servidor: " << respuesta << "\n";
    }

    std::cout << "\nPulsa Enter para volver...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/* ── Logica principal ────────────────────────────────────────────────────── */

void gestionMenuReservas(SocketClient&   sock,
                         CacheOO&        cache,
                         const SesionOO& sesion)
{
    int  opcion = 0;
    bool salir  = false;

    while (!salir)
    {
        crearMenuReservas();

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
                gestionVerReservas(sock, cache, sesion);
                break;

            case 2:
                gestionCancelarReserva(sock, cache, sesion);
                break;

            case 3:
                salir = true;
                break;

            default:
                std::cout << "Opcion invalida.\n";
                break;
        }
    }
}
