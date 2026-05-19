#include "menu_negocios.h"
#include "Protocol.h"
#include "NegocioFactory.h"

#include <iostream>
#include <limits>
#include <vector>
#include <string>
#include <algorithm> // Requerido para std::copy_if
#include <iterator>  // Requerido para std::back_inserter

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
}

static void mostrarTodos(SocketClient& sock, CacheOO& cache)
{
    std::cout << "\nCargando negocios del servidor...\n";
    cargarNegocios(sock, cache);

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

static void mostrarPorTipo(SocketClient& sock, CacheOO& cache, TipoNegocio tipo)
{
    std::cout << "\nCargando negocios del servidor...\n";
    cargarNegocios(sock, cache);

    if (cache.getTotalNegocios() == 0)
    {
        std::cout << "No hay negocios disponibles.\n";
    }
    else
    {
        std::cout << "\n--- Mostrando negocios del tipo: " << tipoAString(tipo) << " ---\n";

        // 1. Obtenemos la lista completa de la cache usando el nuevo método público
        const std::vector<NegocioOO*>& todosLosNegocios = cache.getNegocios();

        // 2. Creamos un vector temporal para guardar los elementos filtrados
        std::vector<NegocioOO*> negociosFiltrados;

        for(int i = 0; i < todosLosNegocios.size(); i++){
        	if(tipo == todosLosNegocios[i]->getTipoEnum()){
        		negociosFiltrados.push_back(todosLosNegocios[i]);
        	}
        }

        // 4. Recorremos e imprimimos el resultado de tu filtrado algorítmico
        if (negociosFiltrados.empty())
        {
            std::cout << "No se encontraron negocios del tipo \"" << tipoAString(tipo) << "\".\n";
        }
        else
        {
            for (NegocioOO* negocio : negociosFiltrados)
            {
                negocio->mostrar(); // Llama al método mostrar de cada objeto individual
            }
        }
    }

    std::cout << "\nPulsa Enter para volver...\n";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}


/* ── Logica principal ────────────────────────────────────────────────────── */

void gestionMenuNegocios(SocketClient& sock, CacheOO& cache)
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
                mostrarPorTipo(sock, cache, TIPO_ACTIVIDAD);
                break;
            case 2:
                mostrarPorTipo(sock, cache, TIPO_RESTAURANTE);
                break;
            case 3:
                mostrarPorTipo(sock, cache, TIPO_SERVICIO);
                break;
            case 4:
                mostrarTodos(sock, cache);
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
