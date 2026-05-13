
#include "Restaurante.h"

#include <iostream>
#include <string>

/* ── Constructor ────────────────────────────────────────────────────────── */

Restaurante::Restaurante(int                id,
                         const std::string& nombre,
                         const std::string& municipio,
                         const std::string& horaApertura,
                         const std::string& horaCierre,
                         int                capacidad,
                         const std::string& tipoCocina,
                         const std::string& menuDelDia)
    : NegocioOO(id, nombre, municipio, horaApertura, horaCierre, capacidad),
      _tipoCocina(tipoCocina),
      _menuDelDia(menuDelDia)
{
    /* cuerpo vacio: la lista de inicializacion asigna todos los campos */
}

/* ── Destructor ─────────────────────────────────────────────────────────── */

Restaurante::~Restaurante()
{
    /* sin recursos dinamicos propios: cuerpo vacio */
}

/* ── Override: getTipo ──────────────────────────────────────────────────── */

std::string Restaurante::getTipo() const
{
    return "restaurante";
}

/* ── Override: mostrar ──────────────────────────────────────────────────── */

void Restaurante::mostrar() const
{
    /* Bloque comun: [restaurante] nombre / municipio / horario / capacidad */
    mostrarComun();

    /* Campos propios del restaurante */
    std::cout << "Tipo cocina: " << _tipoCocina << "\n";

    if (!_menuDelDia.empty())
    {
        std::cout << "Menu del dia: " << _menuDelDia << "\n";
    }
    else
    {
        std::cout << "Menu del dia: no disponible\n";
    }

    std::cout << "-------------------------\n";
}

/* ── Getters propios ────────────────────────────────────────────────────── */

const std::string& Restaurante::getTipoCocina() const
{
    return _tipoCocina;
}

const std::string& Restaurante::getMenuDelDia() const
{
    return _menuDelDia;
}

/* ── Setters propios ────────────────────────────────────────────────────── */

void Restaurante::setTipoCocina(const std::string& tipoCocina)
{
    _tipoCocina = tipoCocina;
}

void Restaurante::setMenuDelDia(const std::string& menuDelDia)
{
    _menuDelDia = menuDelDia;
}
