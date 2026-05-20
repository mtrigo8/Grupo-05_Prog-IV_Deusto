
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
                     const std::string& descripcion,
                     const std::string& dias)
    : NegocioOO(id, nombre, municipio, horaApertura, horaCierre, capacidad),
      _descripcion(descripcion),
      _dias(dias)
{
    /* _plazasOcupadas se inicializa a 0 en NegocioOO y se actualiza desde
     * la cache de reservas cuando se carga el listado de negocios          */
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
    /* Bloque comun: [actividad] nombre / municipio / horario /
     * "Plazas: X libres / Y totales"                          */
    mostrarComun();

    /* Descripcion de la actividad */
    if (!_descripcion.empty())
    {
        std::cout << "Descripcion : " << _descripcion << "\n";
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

const std::string& Actividad::getDescripcion() const
{
    return _descripcion;
}

const std::string& Actividad::getDias() const
{
    return _dias;
}

/* ── Setters propios ────────────────────────────────────────────────────── */

void Actividad::setDescripcion(const std::string& descripcion)
{
    _descripcion = descripcion;
}

void Actividad::setDias(const std::string& dias)
{
    _dias = dias;
}
