#include "Cache.h"

#include <iostream>

/* ── Constructor ────────────────────────────────────────────────────────── */

CacheOO::CacheOO()
{

}

/* ── Destructor ─────────────────────────────────────────────────────────── */

CacheOO::~CacheOO()
{
    limpiarTodo();
}

/* =====================================================================
 * NEGOCIOS
 * ===================================================================== */

/* ── agregarNegocio ─────────────────────────────────────────────────────── */

void CacheOO::agregarNegocio(NegocioOO* negocio)
{
    if (negocio != nullptr)
    {
        _negocios.push_back(negocio);
    }
}

/* ── limpiarNegocios ────────────────────────────────────────────────────── */

void CacheOO::limpiarNegocios()
{
    for (int i = 0; i < (int)_negocios.size(); i++)
    {
        delete _negocios[i];
        _negocios[i] = nullptr;
    }

    _negocios.clear();
}

/* ── getTotalNegocios ───────────────────────────────────────────────────── */

int CacheOO::getTotalNegocios() const
{
    return (int)_negocios.size();
}

/* ── getNegocio ─────────────────────────────────────────────────────────── */

NegocioOO* CacheOO::getNegocio(int indice) const
{
    if (indice < 0 || indice >= (int)_negocios.size())
    {
        return nullptr;
    }

    return _negocios[indice];
}

/* ── buscarNegocioPorId ─────────────────────────────────────────────────── */

NegocioOO* CacheOO::buscarNegocioPorId(int id) const
{
    for (int i = 0; i < (int)_negocios.size(); i++)
    {
        if (_negocios[i]->getId() == id)
        {
            return _negocios[i];
        }
    }

    return nullptr;
}

/* ── mostrarNegocios ────────────────────────────────────────────────────── */

void CacheOO::mostrarNegocios() const
{
    if (_negocios.empty())
    {
        std::cout << "No hay servicios en cache.\n";
        return;
    }

    std::cout << "=== Servicios disponibles (" << _negocios.size() << ") ===\n";

    for (int i = 0; i < (int)_negocios.size(); i++)
    {
        _negocios[i]->mostrar();
    }
}


std::vector<NegocioOO*>& CacheOO::getNegocios() {
	    return _negocios;
}


/* ── mostrarNegociosFiltrados ───────────────────────────────────────────── */

void CacheOO::mostrarNegociosFiltrados(const std::string& tipo) const
{
    int encontrados = 0;

    for (int i = 0; i < (int)_negocios.size(); i++)
    {

        if (_negocios[i]->getTipo() == tipo)
        {
            _negocios[i]->mostrar();
            encontrados++;
        }
    }

    if (encontrados == 0)
    {
        std::cout << "No hay servicios de tipo \"" << tipo << "\" en cache.\n";
    }
}

/* =====================================================================
 * RESERVAS
 * ===================================================================== */

/* ── agregarReserva ─────────────────────────────────────────────────────── */

void CacheOO::agregarReserva(Reserva* reserva)
{
    if (reserva != nullptr)
    {
        _reservas.push_back(reserva);
    }
}

/* ── limpiarReservas ────────────────────────────────────────────────────── */

void CacheOO::limpiarReservas()
{
    for (int i = 0; i < (int)_reservas.size(); i++)
    {
        delete _reservas[i];
        _reservas[i] = nullptr;
    }

    _reservas.clear();
}

/* ── getTotalReservas ───────────────────────────────────────────────────── */

int CacheOO::getTotalReservas() const
{
    return (int)_reservas.size();
}

/* ── getReserva ─────────────────────────────────────────────────────────── */

Reserva* CacheOO::getReserva(int indice) const
{
    if (indice < 0 || indice >= (int)_reservas.size())
    {
        return nullptr;
    }

    return _reservas[indice];
}

/* ── buscarReservaPorId ─────────────────────────────────────────────────── */

Reserva* CacheOO::buscarReservaPorId(int idReserva) const
{
    for (int i = 0; i < (int)_reservas.size(); i++)
    {
        if (_reservas[i]->getIdReserva() == idReserva)
        {
            return _reservas[i];
        }
    }

    return nullptr;
}

/* ── mostrarReservas ────────────────────────────────────────────────────── */

void CacheOO::mostrarReservas() const
{
    if (_reservas.empty())
    {
        std::cout << "No tienes reservas activas.\n";
        return;
    }

    std::cout << "=== Tus reservas (" << _reservas.size() << ") ===\n";

    for (int i = 0; i < (int)_reservas.size(); i++)
    {
        _reservas[i]->mostrar();
    }
}

/* ── contarReservasPorServicio ──────────────────────────────────────────── */

int CacheOO::contarReservasPorServicio(int idServicio) const
{
	int count = 0;

	    for (int i = 0; i < (int)_reservas.size(); i++)
	    {
	        if (_reservas[i]->getIdServicio() == idServicio && _reservas[i]->estaActiva())
	        {
	            count++;
	        }
	    }

	    return count;
}

/* =====================================================================
 * UTILIDAD GENERAL
 * ===================================================================== */

/* ── limpiarTodo ────────────────────────────────────────────────────────── */

void CacheOO::limpiarTodo()
{
    limpiarNegocios();
    limpiarReservas();
}
