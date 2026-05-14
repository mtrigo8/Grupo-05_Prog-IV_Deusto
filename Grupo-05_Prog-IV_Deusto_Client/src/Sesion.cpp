#include "Sesion.h"

#include <string>

/* ── Constructor ────────────────────────────────────────────────────────── */

SesionOO::SesionOO()
    : _idUsuario(-1),
      _nombre(""),
      _rol(""),
      _autenticado(false)
{
}

/* ── Destructor ─────────────────────────────────────────────────────────── */

SesionOO::~SesionOO()
{
}

/* ── iniciarSesion ──────────────────────────────────────────────────────── */

void SesionOO::iniciarSesion(int                idUsuario,
                              const std::string& nombre,
                              const std::string& rol)
{
    _idUsuario   = idUsuario;
    _nombre      = nombre;
    _rol         = rol;
    _autenticado = true;
}

/* ── cerrarSesion ───────────────────────────────────────────────────────── */

void SesionOO::cerrarSesion()
{
    _idUsuario   = -1;
    _nombre      = "";
    _rol         = "";
    _autenticado = false;
}

/* ── estaAutenticado ────────────────────────────────────────────────────── */

bool SesionOO::estaAutenticado() const
{
    return _autenticado;
}

/* ── getId ──────────────────────────────────────────────────────────────── */

int SesionOO::getId() const
{
    return _idUsuario;
}

/* ── getNombre ──────────────────────────────────────────────────────────── */

const std::string& SesionOO::getNombre() const
{
    return _nombre;
}

/* ── getRol ─────────────────────────────────────────────────────────────── */

const std::string& SesionOO::getRol() const
{
    return _rol;
}
