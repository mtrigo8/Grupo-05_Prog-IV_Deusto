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
                parsed.descripcion, /* tipoCocina viene de descripcion en BD */
                ""                  /* menuDelDia: no almacenado en BD       */
            );

        case TIPO_ACTIVIDAD:

            return new Actividad(
                parsed.id,
                parsed.nombre,
                parsed.municipio,
                parsed.horaApertura,
                parsed.horaCierre,
                capacidad,
                parsed.dias,  /* dias de la actividad */
                0             /* plazasOcupadas: se calcula en servidor     */
            );

        case TIPO_SERVICIO:

            return new ServicioMunicipal(
                parsed.id,
                parsed.nombre,
                parsed.municipio,
                parsed.horaApertura,
                parsed.horaCierre,
                capacidad,
                parsed.descripcion, /* descripcion viene directamente de BD */
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
                parsed.descripcion,
                parsed.dias
            );
    }
}

/* ── crearDesdeLinea ────────────────────────────────────────────────────── */

NegocioOO* NegocioFactory::crearDesdeLinea(const std::string& linea,
                                            int                capacidad)
{
    ParsedServicio parsed = parseServicio(linea);

    /* Si el caller no pasa capacidad explícita (0) usamos la del protocolo */
    int cap = (capacidad != 0) ? capacidad : parsed.capacidad;

    return crear(parsed, cap);
}
