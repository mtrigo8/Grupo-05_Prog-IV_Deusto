
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
                         const std::string& tipoCocina)
    : NegocioOO(id, nombre, municipio, horaApertura, horaCierre, capacidad),
      _tipoCocina(tipoCocina)
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
    /* Bloque comun: [restaurante] nombre / municipio / horario /
     * "Plazas: X libres / Y totales"                          */
    mostrarComun();

    /* Campo propio del restaurante */
    std::cout << "Tipo cocina : " << _tipoCocina << "\n";

    std::cout << "-------------------------\n";
}

/* ── Getters propios ────────────────────────────────────────────────────── */

const std::string& Restaurante::getTipoCocina() const
{
    return _tipoCocina;
}

/* ── Setters propios ────────────────────────────────────────────────────── */

void Restaurante::setTipoCocina(const std::string& tipoCocina)
{
    _tipoCocina = tipoCocina;
}
