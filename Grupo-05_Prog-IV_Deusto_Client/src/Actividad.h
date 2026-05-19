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
              int                plazasOcupadas = 0,
              const std::string& dias = "",
              const std::string& descripcion = ""); // Añadida descripcion

    /* ── Destructor ──────────────────────────────────────────────────────── */
    ~Actividad() override;

    /* ── Overrides de NegocioOO ──────────────────────────────────────────── */
    void mostrar() const override;

    std::string getTipo() const override;

    /* ── Getters propios ─────────────────────────────────────────────────── */
    const std::string& getDias()        const;
    const std::string& getDescripcion() const;

    /* ── Setters propios ─────────────────────────────────────────────────── */
    void setDias(const std::string& dias);
    void setDescripcion(const std::string& descripcion);

private:

    std::string _dias;
    std::string _descripcion; // Sustituye a _plazasOcupadas
};

#endif /* ACTIVIDAD_H_ */
