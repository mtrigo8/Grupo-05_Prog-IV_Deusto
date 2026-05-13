
#include "ServicioMunicipal.h"

#include <iostream>
#include <string>

/* ── Constructor ────────────────────────────────────────────────────────── */

ServicioMunicipal::ServicioMunicipal(int                id,
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
    /* cuerpo vacio: la lista de inicializacion asigna todos los campos */
}

/* ── Destructor ─────────────────────────────────────────────────────────── */

ServicioMunicipal::~ServicioMunicipal()
{
    /* sin recursos dinamicos propios: cuerpo vacio */
}

/* ── Override: getTipo ──────────────────────────────────────────────────── */

std::string ServicioMunicipal::getTipo() const
{
    return "servicio";
}

/* ── Override: mostrar ──────────────────────────────────────────────────── */

void ServicioMunicipal::mostrar() const
{
    /* Bloque comun: [servicio] nombre / municipio / horario / capacidad */
    mostrarComun();

    /* Campos propios del servicio municipal */
    std::cout << "Descripcion: " << _descripcion << "\n";

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

const std::string& ServicioMunicipal::getDescripcion() const
{
    return _descripcion;
}

const std::string& ServicioMunicipal::getDias() const
{
    return _dias;
}

/* ── Setters propios ────────────────────────────────────────────────────── */

void ServicioMunicipal::setDescripcion(const std::string& descripcion)
{
    _descripcion = descripcion;
}

void ServicioMunicipal::setDias(const std::string& dias)
{
    _dias = dias;
}
