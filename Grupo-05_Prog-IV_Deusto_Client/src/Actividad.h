
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
              const std::string& descripcion = "",
              const std::string& dias = "");

    /* ── Destructor ──────────────────────────────────────────────────────── */
    ~Actividad() override;

    /* ── Overrides de NegocioOO ──────────────────────────────────────────── */
    void mostrar() const override;

    std::string getTipo() const override;

    /* ── Getters propios ─────────────────────────────────────────────────── */
    const std::string& getDescripcion() const;
    const std::string& getDias()        const;

    /* ── Setters propios ─────────────────────────────────────────────────── */
    void setDescripcion(const std::string& descripcion);
    void setDias(const std::string& dias);

private:

    std::string _descripcion;
    std::string _dias;
};

#endif /* ACTIVIDAD_H_ */
