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
                     int                plazasOcupadas,
                     const std::string& dias,
                     const std::string& descripcion)
    : NegocioOO(id, nombre, municipio, horaApertura, horaCierre, capacidad, plazasOcupadas),
      _dias(dias),
      _descripcion(descripcion)
{
}

/* ── Destructor ─────────────────────────────────────────────────────────── */

Actividad::~Actividad()
{
}

/* ── Override: getTipo ──────────────────────────────────────────────────── */

std::string Actividad::getTipo() const
{
    return "actividad";
}

/* ── Override: mostrar ──────────────────────────────────────────────────── */

void Actividad::mostrar() const
{
    /* Bloque comun: [actividad] nombre / municipio / horario / plazas */
    mostrarComun();

    /* Descripcion de la actividad */
    if (!_descripcion.empty())
    {
        std::cout << "Descripcion : " << _descripcion << "\n";
    }
    else
    {
        std::cout << "Descripcion : no especificada\n";
    }

    /* Dias en que se celebra la actividad */
    if (!_dias.empty())
    {
        std::cout << "Dias        : " << _dias << "\n";
    }
    else
    {
        std::cout << "Dias        : no especificados\n";
    }

    std::cout << "-------------------------\n";
}

/* ── Getters propios ────────────────────────────────────────────────────── */

const std::string& Actividad::getDias() const { return _dias; }
const std::string& Actividad::getDescripcion() const { return _descripcion; }

/* ── Setters propios ────────────────────────────────────────────────────── */

void Actividad::setDias(const std::string& dias) { _dias = dias; }
void Actividad::setDescripcion(const std::string& descripcion) { _descripcion = descripcion; }
