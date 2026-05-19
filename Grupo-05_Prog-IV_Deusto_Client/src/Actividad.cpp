
#include "Actividad.h"

#include <iostream>
#include <string>

/* ── Constructor ────────────────────────────────────────────────────────── */

Actividad::Actividad(int                id,
                     const std::string& nombre,
                     const std::string& municipio,
                     const std::string& horaApertura,
                     const std::string& horaCierre,
                     int                capacidad,
                     const std::string& dias,
                     int                plazasOcupadas)
    : NegocioOO(id, nombre, municipio, horaApertura, horaCierre, capacidad),
      _dias(dias),
      _plazasOcupadas(plazasOcupadas)
{
    /* cuerpo vacio: la lista de inicializacion asigna todos los campos */
}

/* ── Destructor ─────────────────────────────────────────────────────────── */

Actividad::~Actividad()
{
    /* sin recursos dinamicos propios: cuerpo vacio */
}

/* ── Override: getTipo ──────────────────────────────────────────────────── */

std::string Actividad::getTipo() const
{
    return "actividad";
}

/* ── Override: mostrar ──────────────────────────────────────────────────── */

void Actividad::mostrar() const
{
    /* Bloque comun: [actividad] nombre / municipio / horario / capacidad */
    mostrarComun();

    /* Dias en que se celebra la actividad */
    if (!_dias.empty())
    {
        std::cout << "Dias        : " << _dias << "\n";
    }
    else
    {
        std::cout << "Dias        : no especificados\n";
    }

    /* Plazas disponibles */
    std::cout << "Plazas      : " << getPlazasLibres()
              << " libres / " << _capacidad << " totales\n";

    std::cout << "-------------------------\n";
}

/* ── Getters propios ────────────────────────────────────────────────────── */

const std::string& Actividad::getDias() const
{
    return _dias;
}

int Actividad::getPlazasOcupadas() const
{
    return _plazasOcupadas;
}

int Actividad::getPlazasLibres() const
{
    return _capacidad - _plazasOcupadas;
}

/* ── Setters propios ────────────────────────────────────────────────────── */

void Actividad::setDias(const std::string& dias)
{
    _dias = dias;
}

void Actividad::setPlazasOcupadas(int plazasOcupadas)
{
    _plazasOcupadas = plazasOcupadas;
}
