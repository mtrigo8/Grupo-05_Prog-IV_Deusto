#ifndef SERVICIOMUNICIPAL_H_
#define SERVICIOMUNICIPAL_H_

#include "Negocio.h"
#include <string>

class ServicioMunicipal : public NegocioOO
{
public:

    /* ── Constructor ─────────────────────────────────────────────────────── */
    ServicioMunicipal(int                id,
                      const std::string& nombre,
                      const std::string& municipio,
                      const std::string& horaApertura,
                      const std::string& horaCierre,
                      int                capacidad,
                      int                plazasOcupadas,
                      const std::string& descripcion,
                      const std::string& dias = "");

    /* ── Destructor ──────────────────────────────────────────────────────── */
    ~ServicioMunicipal() override;

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
    std::string _dias;          /* dias de apertura separados por coma */
};

#endif /* SERVICIOMUNICIPAL_H_ */
