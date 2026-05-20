#include "menu_negocios.h"
#include "Protocol.h"
#include "NegocioFactory.h"
#include "menu_reservas.h"

#include <iostream>
#include <limits>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>

/* ── Vista ───────────────────────────────────────────────────────────────── */

void crearMenuNegocios()
{
    std::cout << "\n=======================\n"
              << "   Tipos de Negocio\n"
              << "=======================\n"
              << "1. Ver Actividades\n"
              << "2. Ver Restaurantes\n"
              << "3. Ver Servicios\n"
              << "4. Ver TODOS los negocios\n"
              << "5. Volver\n"
              << "=======================\n";
}

/* ── Helpers internos ────────────────────────────────────────────────────── */

/**
 * Carga todos los negocios del servidor.
 * NO carga reservas — eso es responsabilidad del llamador si lo necesita.
 * Sincroniza plazas ocupadas con las reservas que ya estén en caché.
 *
 * FIX: Se eliminó la llamada a cargarReservas() desde aquí para evitar
 * enviar dos peticiones seguidas al servidor (GET_RESERVA + GET_SERVICIOS)
 * sin esperar la respuesta completa de cada una, lo que causaba que los
 * frames de ambas listas se mezclaran en el buffer TCP y el cliente los
 * leyera desordenados.
 */
static void cargarNegocios(SocketClient& sock, CacheOO& cache, const SesionOO& sesion)
{
    /* ELIMINADO: cargarReservas(sock, cache, sesion);
     * Las reservas deben cargarse por separado ANTES de llamar a esta
     * función si se necesita sincronización de plazas. */

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

    if (lineas.size() == 1 && esError(lineas[0]))
    {
        std::cout << "Error al obtener negocios: " << lineas[0] << "\n";
        return;
    }

    std::vector<ParsedServicio> parsed = parseListaServicios(lineas);

    for (int i = 0; i < (int)parsed.size(); i++)
    {
        NegocioOO* obj = NegocioFactory::crear(parsed[i], parsed[i].capacidad);
        cache.agregarNegocio(obj);
    }

    std::cout << cache.getTotalNegocios() << " negocios cargados desde el servidor.\n";

    /* Sincronizar plazas ocupadas con la cache de reservas del usuario.
     * Si el usuario tiene reservas ya cargadas en cache, cada negocio
     * mostrara correctamente cuantas plazas estan ocupadas.           */
    for (int i = 0; i < cache.getTotalNegocios(); i++)
    {
        NegocioOO* n = cache.getNegocio(i);
        int ocupadas = cache.contarReservasPorServicio(n->getId());
        n->setPlazasOcupadas(ocupadas);
    }
}

/**
 * Carga reservas y luego negocios, en ese orden, esperando la respuesta
 * completa de cada petición antes de enviar la siguiente.
 * Esta es la función que debe usarse cuando se necesitan ambos datos.
 */
static void cargarReservasYNegocios(SocketClient& sock, CacheOO& cache, const SesionOO& sesion)
{
    cargarReservas(sock, cache, sesion);   /* 1º: GET_RESERVA completo      */
    cargarNegocios(sock, cache, sesion);   /* 2º: GET_SERVICIOS completo     */
}

static void mostrarTodos(SocketClient& sock, CacheOO& cache, const SesionOO& sesion)
{
    std::cout << "\nCargando negocios del servidor...\n";
    cargarReservasYNegocios(sock, cache, sesion);

    if (cache.getTotalNegocios() == 0)
    {
        std::cout << "No hay negocios disponibles.\n";
    }
    else
    {
        cache.mostrarNegocios();
    }

    std::cout << "\nPulsa Enter para volver...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static void mostrarPorTipo(SocketClient& sock, CacheOO& cache, TipoNegocio tipo, const SesionOO& sesion)
{
    std::cout << "\nCargando negocios del servidor...\n";
    cargarReservasYNegocios(sock, cache, sesion);

    if (cache.getTotalNegocios() == 0)
    {
        std::cout << "No hay negocios disponibles.\n";
    }
    else
    {
        std::cout << "\n--- Mostrando negocios del tipo: " << tipoAString(tipo) << " ---\n";

        const std::vector<NegocioOO*>& todosLosNegocios = cache.getNegocios();

        std::vector<NegocioOO*> negociosFiltrados;

        for (int i = 0; i < (int)todosLosNegocios.size(); i++)
        {
            if (tipo == todosLosNegocios[i]->getTipoEnum())
            {
                negociosFiltrados.push_back(todosLosNegocios[i]);
            }
        }

        if (negociosFiltrados.empty())
        {
            std::cout << "No se encontraron negocios del tipo \"" << tipoAString(tipo) << "\".\n";
        }
        else
        {
            for (int i = 0; i < (int)negociosFiltrados.size(); i++)
            {
                negociosFiltrados[i]->mostrar();
            }
        }
    }

    std::cout << "\nPulsa Enter para volver...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


/* ── Logica principal ────────────────────────────────────────────────────── */

void gestionMenuNegocios(SocketClient& sock, CacheOO& cache, const SesionOO& sesion)
{
    int  opcion = 0;
    bool salir  = false;

    while (!salir)
    {
        crearMenuNegocios();

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
                mostrarPorTipo(sock, cache, TIPO_ACTIVIDAD, sesion);
                break;
            case 2:
                mostrarPorTipo(sock, cache, TIPO_RESTAURANTE, sesion);
                break;
            case 3:
                mostrarPorTipo(sock, cache, TIPO_SERVICIO, sesion);
                break;
            case 4:
                mostrarTodos(sock, cache, sesion);
                break;
            case 5:
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
