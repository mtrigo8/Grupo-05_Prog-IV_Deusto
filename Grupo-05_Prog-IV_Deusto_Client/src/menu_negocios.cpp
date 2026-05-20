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
 * Carga todos los negocios del servidor y sincroniza las plazas ocupadas
 * con la cache de reservas del usuario actual.
 */
static void cargarNegocios(SocketClient& sock, CacheOO& cache, const SesionOO& sesion)
{
    cargarReservas(sock, cache, sesion);
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
     * Si el usuario tiene reservas cargadas, cada negocio mostrara
     * correctamente cuantas plazas estan ocupadas.                   */
    for (int i = 0; i < cache.getTotalNegocios(); i++)
    {
        NegocioOO* n = cache.getNegocio(i);
        int ocupadas = cache.contarReservasPorServicio(n->getId());
        n->setPlazasOcupadas(ocupadas);
    }
}

static void mostrarTodos(SocketClient& sock, CacheOO& cache, const SesionOO& sesion)
{
    std::cout << "\nCargando negocios del servidor...\n";
    cargarNegocios(sock, cache, sesion);

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
    cargarNegocios(sock, cache, sesion);

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
