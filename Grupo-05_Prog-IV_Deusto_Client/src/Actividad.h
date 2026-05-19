
#ifndef ACTIVIDAD_H_
#define ACTIVIDAD_H_

#include "Negocio.h"
#include <string>

class Actividad : public NegocioOO
{
public:

    /* ── Constructor ─────────────────────────────────────────────────────── */
    Actividad(int                id,
              const std::string& nombre,
              const std::string& municipio,
              const std::string& horaApertura,
              const std::string& horaCierre,
              int                capacidad,
              const std::string& dias = "",
              int                plazasOcupadas = 0);

    /* ── Destructor ──────────────────────────────────────────────────────── */
    ~Actividad() override;

    /* ── Overrides de NegocioOO ──────────────────────────────────────────── */
    void mostrar() const override;

    std::string getTipo() const override;

    /* ── Getters propios ─────────────────────────────────────────────────── */
    const std::string& getDias()          const;
    int                getPlazasOcupadas() const;
    int                getPlazasLibres()   const;   /* capacidad - ocupadas */

    /* ── Setters propios ─────────────────────────────────────────────────── */
    void setDias(const std::string& dias);
    void setPlazasOcupadas(int plazasOcupadas);

private:

    std::string _dias;
    int         _plazasOcupadas;
};

#endif /* ACTIVIDAD_H_ */
