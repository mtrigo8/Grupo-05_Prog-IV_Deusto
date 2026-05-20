#include "menu_reservas.h"
#include "Protocol.h"
#include "Reserva.h"

#include <iostream>
#include <limits>
#include <vector>
#include <string>

/* =========================================================================
 * VISTA
 * ========================================================================= */

void crearMenuReservas()
{
    std::cout << "\n=======================\n"
              << "      Mis reservas\n"
              << "=======================\n"
              << "1. Ver mis reservas\n"
              << "2. Hacer una reserva\n"
              << "3. Cancelar una reserva\n"
              << "4. Modificar una reserva\n"
              << "5. Volver\n"
              << "=======================\n";
}

/* =========================================================================
 * HELPERS INTERNOS
 * ========================================================================= */

/* ── cargarReservas ────────────────────────────────────────────────────── */
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

/* ── gestionVerReservas ───────────────────────────────────────────────────
 * GET_RESERVA: recarga la cache y muestra las reservas del usuario.
 * ------------------------------------------------------------------------- */
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

/* ── gestionHacerReserva ──────────────────────────────────────────────────
 * CREATE_RESERVA: pide un id de servicio y crea la reserva.
 * Actualiza la cache recargandola tras el exito.
 * ------------------------------------------------------------------------- */
static void gestionHacerReserva(SocketClient&   sock,
                                CacheOO&        cache,
                                const SesionOO& sesion)
{
    std::cout << "\n--- Hacer reserva ---\n";
    std::cout << "Introduce el ID del servicio a reservar (0 para volver): ";

    int idServicio = 0;

    if (!(std::cin >> idServicio))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada invalida.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (idServicio == 0)
    {
        return;
    }

    /* Enviar comando y parametros */
    if (!sock.enviar(CMD_CREATE_RESERVA))
    {
        std::cout << "Error: no se pudo enviar el comando al servidor.\n";
        return;
    }

    if (!sock.enviar(buildCreateReserva(sesion.getId(), idServicio)))
    {
        std::cout << "Error: no se pudieron enviar los parametros.\n";
        return;
    }

    std::string respuesta = sock.recibir();

    if (esOk(respuesta))
    {
        std::cout << "Reserva creada correctamente.\n";

        /* Recargar cache para reflejar la nueva reserva */
        cargarReservas(sock, cache, sesion);
    }
    else if (esError(respuesta))
    {
        std::vector<std::string> campos = splitSEP(respuesta);
        std::string motivo = (campos.size() >= 2) ? campos[1] : respuesta;

        if      (motivo == "SIN_CUPOS")     std::cout << "Error: el servicio no tiene cupos disponibles.\n";
        else if (motivo == "YA_RESERVADO")  std::cout << "Error: ya tienes una reserva para ese servicio.\n";
        else if (motivo == "NO_ENCONTRADO") std::cout << "Error: el servicio no existe.\n";
        else                                std::cout << "Error al crear la reserva: " << motivo << "\n";
    }

    std::cout << "\nPulsa Enter para volver...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/* ── gestionCancelarReserva ───────────────────────────────────────────────
 * CANCEL_RESERVA: muestra reservas, pide id y cancela la elegida.
 * Limpia la cache tras el exito.
 * ------------------------------------------------------------------------- */
static void gestionCancelarReserva(SocketClient&   sock,
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

    /* Validar en cache antes de ir al servidor */
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

    /* Enviar comando y parametros */
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

        /* Limpiar cache: ya no esta activa */
        cache.limpiarReservas();
    }
    else if (esError(respuesta))
    {
        std::vector<std::string> campos = splitSEP(respuesta);
        std::string motivo = (campos.size() >= 2) ? campos[1] : respuesta;

        if      (motivo == "NO_CANCELABLE")  std::cout << "Error: la reserva no puede cancelarse.\n";
        else if (motivo == "NO_AUTORIZADO")  std::cout << "Error: no tienes permiso para cancelar esta reserva.\n";
        else if (motivo == "NO_ENCONTRADO")  std::cout << "Error: la reserva no existe en el servidor.\n";
        else                                 std::cout << "Error al cancelar la reserva: " << motivo << "\n";
    }

    std::cout << "\nPulsa Enter para volver...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/* ── gestionModificarReserva ──────────────────────────────────────────────
 * UPDATE_RESERVA: muestra reservas, pide id de reserva e id de nuevo
 * servicio, y actualiza. Recarga la cache tras el exito.
 * ------------------------------------------------------------------------- */
static void gestionModificarReserva(SocketClient&   sock,
                                    CacheOO&        cache,
                                    const SesionOO& sesion)
{
    std::cout << "\nCargando reservas del servidor...\n";
    cargarReservas(sock, cache, sesion);

    if (cache.getTotalReservas() == 0)
    {
        std::cout << "No tienes reservas que modificar.\n";
        std::cout << "\nPulsa Enter para volver...\n";
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }

    cache.mostrarReservas();

    /* Pedir id de la reserva a modificar */
    std::cout << "\nIntroduce el numero de reserva a modificar (0 para volver): ";
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

    /* Validar en cache antes de ir al servidor */
    Reserva* reserva = cache.buscarReservaPorId(idReserva);

    if (reserva == nullptr)
    {
        std::cout << "No se encontro la reserva #" << idReserva << ".\n";
        return;
    }

    if (!reserva->estaActiva())
    {
        std::cout << "La reserva #" << idReserva
                  << " no esta activa y no puede modificarse.\n";
        return;
    }

    /* Pedir el nuevo servicio */
    std::cout << "Introduce el ID del nuevo servicio (0 para volver): ";
    int idNuevoServicio = 0;

    if (!(std::cin >> idNuevoServicio))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Entrada invalida.\n";
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (idNuevoServicio == 0)
    {
        return;
    }

    /* Enviar comando y parametros */
    if (!sock.enviar(CMD_UPDATE_RESERVA))
    {
        std::cout << "Error: no se pudo enviar el comando al servidor.\n";
        return;
    }

    if (!sock.enviar(buildUpdateReserva(idReserva, sesion.getId(), idNuevoServicio)))
    {
        std::cout << "Error: no se pudieron enviar los parametros.\n";
        return;
    }

    std::string respuesta = sock.recibir();

    if (esOk(respuesta))
    {
        std::cout << "Reserva #" << idReserva << " actualizada correctamente.\n";


        /* Recargar cache para reflejar el cambio */
        cargarReservas(sock, cache, sesion);
    }
    else if (esError(respuesta))
    {
        std::vector<std::string> campos = splitSEP(respuesta);
        std::string motivo = (campos.size() >= 2) ? campos[1] : respuesta;

        if      (motivo == "SIN_CUPOS")     std::cout << "Error: el nuevo servicio no tiene cupos disponibles.\n";
        else if (motivo == "YA_RESERVADO")  std::cout << "Error: ya tienes una reserva para ese servicio.\n";
        else if (motivo == "NO_ENCONTRADO") std::cout << "Error: la reserva o el servicio no existe.\n";
        else if (motivo == "NO_AUTORIZADO") std::cout << "Error: no tienes permiso para modificar esta reserva.\n";
        else                                std::cout << "Error al modificar la reserva: " << motivo << "\n";
    }

    std::cout << "\nPulsa Enter para volver...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

/* =========================================================================
 * LOGICA PRINCIPAL
 * ========================================================================= */

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
            case 1: gestionVerReservas     (sock, cache, sesion); break;
            case 2: gestionHacerReserva    (sock, cache, sesion); break;
            case 3: gestionCancelarReserva (sock, cache, sesion); break;
            case 4: gestionModificarReserva(sock, cache, sesion); break;
            case 5: salir = true;                                 break;

            default:
                std::cout << "Opcion invalida.\n";
                std::cout << "Pulsa Enter para continuar...\n";
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                break;
        }
    }
}
