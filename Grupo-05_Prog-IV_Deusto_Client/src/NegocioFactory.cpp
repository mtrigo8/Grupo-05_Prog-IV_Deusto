#include "NegocioFactory.h"

#include "Restaurante.h"
#include "ServicioMunicipal.h"
#include "Actividad.h"

#include <iostream>

/* ── crear ──────────────────────────────────────────────────────────────── */

NegocioOO* NegocioFactory::crear(const ParsedServicio& parsed, int capacidad)
{
    switch (tipoDesdeString(parsed.tipo))
    {
        case TIPO_RESTAURANTE:

            return new Restaurante(
                parsed.id,
                parsed.nombre,
                parsed.municipio,
                parsed.horaApertura,
                parsed.horaCierre,
                capacidad,
                "",   /* tipoCocina: no en protocolo actual */
                ""    /* menuDelDia: no en protocolo actual */
            );

        case TIPO_ACTIVIDAD:

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

        case TIPO_SERVICIO:

            return new ServicioMunicipal(
                parsed.id,
                parsed.nombre,
                parsed.municipio,
                parsed.horaApertura,
                parsed.horaCierre,
                capacidad,
                "",           /* descripcion: no en protocolo actual */
                parsed.dias
            );

        case TIPO_DESCONOCIDO:
        default:

            std::cout << "[NegocioFactory] Tipo desconocido \""
                      << parsed.tipo
                      << "\", se crea como ServicioMunicipal.\n";

            return new ServicioMunicipal(
                parsed.id,
                parsed.nombre,
                parsed.municipio,
                parsed.horaApertura,
                parsed.horaCierre,
                capacidad,
                "",
                parsed.dias
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
