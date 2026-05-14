
#include "Negocio.h"

#include <iostream>
#include <string>

/* ── Constructor ────────────────────────────────────────────────────────── */

NegocioOO::NegocioOO(int                id,
                     const std::string& nombre,
                     const std::string& municipio,
                     const std::string& horaApertura,
                     const std::string& horaCierre,
                     int                capacidad)
    : _id(id),
      _nombre(nombre),
      _municipio(municipio),
      _horaApertura(horaApertura),
      _horaCierre(horaCierre),
      _capacidad(capacidad)
{
}

/* ── Destructor ─────────────────────────────────────────────────────────── */

NegocioOO::~NegocioOO()
{
}

/* ── getTipoEnum ────────────────────────────────────────────────────────── */

TipoNegocio NegocioOO::getTipoEnum() const
{

    return tipoDesdeString(getTipo());
}

/* ── Getters ────────────────────────────────────────────────────────────── */

int NegocioOO::getId() const
{
    return _id;
}

const std::string& NegocioOO::getNombre() const
{
    return _nombre;
}

const std::string& NegocioOO::getMunicipio() const
{
    return _municipio;
}

const std::string& NegocioOO::getHoraApertura() const
{
    return _horaApertura;
}

const std::string& NegocioOO::getHoraCierre() const
{
    return _horaCierre;
}

int NegocioOO::getCapacidad() const
{
    return _capacidad;
}

/* ── Setters ────────────────────────────────────────────────────────────── */

void NegocioOO::setCapacidad(int capacidad)
{
    _capacidad = capacidad;
}

void NegocioOO::setHoraApertura(const std::string& horaApertura)
{
    _horaApertura = horaApertura;
}

void NegocioOO::setHoraCierre(const std::string& horaCierre)
{
    _horaCierre = horaCierre;
}

/* ── mostrarComun ───────────────────────────────────────────────────────── */

void NegocioOO::mostrarComun() const
{
    std::cout << "-------------------------\n"
              << "[" << getTipo() << "] " << _nombre << "\n"
              << "Municipio  : " << _municipio    << "\n"
              << "Horario    : " << _horaApertura << " - " << _horaCierre << "\n"
              << "Capacidad  : " << _capacidad    << " plazas\n"
              << "-------------------------\n";
}
