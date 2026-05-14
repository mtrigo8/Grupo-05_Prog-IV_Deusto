#include "NegocioFactory.h"

#include "Restaurante.h"
#include "ServicioMunicipal.h"
#include "Actividad.h"

#include <iostream>

/* ── crear ──────────────────────────────────────────────────────────────── */

NegocioOO* NegocioFactory::crear(const ParsedServicio& parsed, int capacidad)
{
    if (parsed.tipo == "restaurante")
    {
        return new Restaurante(
            parsed.id,
            parsed.nombre,
            parsed.municipio,
            parsed.horaApertura,
            parsed.horaCierre,
            capacidad,
            "",   /* tipoCocina: no disponible en protocolo actual */
            ""    /* menuDelDia: no disponible en protocolo actual */
        );
    }
    else if (parsed.tipo == "actividad")
    {
        return new Actividad(
            parsed.id,
            parsed.nombre,
            parsed.municipio,
            parsed.horaApertura,
            parsed.horaCierre,
            capacidad,
            parsed.dias,  /* fecha/dias de la actividad */
            0             /* plazasOcupadas: se actualiza tras reservar */
        );
    }
    else
    {

        if (parsed.tipo != "servicio" && !parsed.tipo.empty())
        {
            std::cout << "[NegocioFactory] Tipo desconocido \""
                      << parsed.tipo
                      << "\", se crea como ServicioMunicipal.\n";
        }

        return new ServicioMunicipal(
            parsed.id,
            parsed.nombre,
            parsed.municipio,
            parsed.horaApertura,
            parsed.horaCierre,
            capacidad,
            "",           /* descripcion: no disponible en protocolo actual */
            parsed.dias   /* dias de apertura */
        );
    }
}

/* ── crearDesdeLinea ────────────────────────────────────────────────────── */

NegocioOO* NegocioFactory::crearDesdeLinea(const std::string& linea,
                                            int                capacidad)
{
    ParsedServicio parsed = parseServicio(linea);
    return crear(parsed, capacidad);
}
