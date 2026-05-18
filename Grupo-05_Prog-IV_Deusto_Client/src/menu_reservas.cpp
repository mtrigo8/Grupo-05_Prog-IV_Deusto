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
              << "2. Hacer una reserva\n"
              << "3. Cancelar una reserva\n"
              << "4. Volver\n"
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

/* Nota: Mantengo esta función antigua por si necesitas reciclar su lógica
   dentro de tu nueva clase/función de borrar más adelante. */
static void gestionCancelarReservaOld(SocketClient&   sock,
                                      CacheOO&        cache,
                                      const SesionOO& sesion)
{
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
        cache.limpiarReservas();
    }
    else if (esError(respuesta))
    {
        std::vector<std::string> campos = splitSEP(respuesta);
        std::string motivo = (campos.size() >= 2) ? campos[1] : respuesta;

        if (motivo == "NO_CANCELABLE") std::cout << "Error: la reserva no puede cancelarse.\n";
        else if (motivo == "NO_AUTORIZADO") std::cout << "Error: no tienes permiso.\n";
        else if (motivo == "NO_ENCONTRADO") std::cout << "Error: la reserva no existe.\n";
        else std::cout << "Error al cancelar la reserva: " << motivo << "\n";
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
                // =============================================================
                // DESCOMENTAR CUANDO ESTE CREADA LA CLASE/FUNCIÓN:
                // menu_hacer_reserva(sock, cache, sesion);
                // =============================================================
                std::cout << "\n[Info] La opcion de hacer reserva esta en construccion.\n";
                std::cout << "Pulsa Enter para continuar...\n";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;

            case 3:
                // =============================================================
                // DESCOMENTAR CUANDO ESTE CREADA LA CLASE/FUNCIÓN:
                // menu_borrar_reserva(sock, cache, sesion);
                // =============================================================
                std::cout << "\n[Info] La opcion de borrar reserva esta en construccion.\n";
                std::cout << "Pulsa Enter para continuar...\n";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;

            case 4:
                salir = true;
                break;

            default:
                std::cout << "Opcion invalida.\n";
                std::cout << "Pulsa Enter para continuar...\n";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
        }
    }
}
