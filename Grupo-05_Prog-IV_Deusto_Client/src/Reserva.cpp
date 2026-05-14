#include "Reserva.h"

#include <iostream>
#include <string>

/* ── Constructor ────────────────────────────────────────────────────────── */

Reserva::Reserva(int                idReserva,
                 int                idUsuario,
                 int                idServicio,
                 const std::string& fechaRegistro,
                 const std::string& nombreServicio,
                 const std::string& estado)
    : _idReserva(idReserva),
      _idUsuario(idUsuario),
      _idServicio(idServicio),
      _fechaRegistro(fechaRegistro),
      _nombreServicio(nombreServicio),
      _estado(estado)
{
}

/* ── Destructor ─────────────────────────────────────────────────────────── */

Reserva::~Reserva()
{
}

/* ── mostrar ────────────────────────────────────────────────────────────── */

void Reserva::mostrar() const
{
    std::cout << "-------------------------\n"
              << "Reserva #"   << _idReserva << "\n"
              << "Servicio     : " << _nombreServicio
              << "  (id: "     << _idServicio << ")\n"
              << "Fecha        : " << _fechaRegistro  << "\n"
              << "Estado       : " << _estado         << "\n"
              << "-------------------------\n";
}

/* ── Getters ────────────────────────────────────────────────────────────── */

int Reserva::getIdReserva() const
{
    return _idReserva;
}

int Reserva::getIdUsuario() const
{
    return _idUsuario;
}

int Reserva::getIdServicio() const
{
    return _idServicio;
}

const std::string& Reserva::getFechaRegistro() const
{
    return _fechaRegistro;
}

const std::string& Reserva::getNombreServicio() const
{
    return _nombreServicio;
}

const std::string& Reserva::getEstado() const
{
    return _estado;
}

/* ── Setters ────────────────────────────────────────────────────────────── */

void Reserva::setEstado(const std::string& estado)
{
    _estado = estado;
}

/* ── Helper de estado ───────────────────────────────────────────────────── */

bool Reserva::estaActiva() const
{
    return _estado == "ACTIVA";
}
