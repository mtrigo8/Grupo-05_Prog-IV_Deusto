#ifndef RESTAURANTE_H_
#define RESTAURANTE_H_

#include "Negocio.h"
#include <string>

class Restaurante : public NegocioOO
{
public:

    /* ── Constructor ─────────────────────────────────────────────────────── */
    Restaurante(int                id,
                const std::string& nombre,
                const std::string& municipio,
                const std::string& horaApertura,
                const std::string& horaCierre,
                int                capacidad,
                int                plazasOcupadas,
                const std::string& tipoCocina); // Eliminado menuDelDia

    /* ── Destructor ──────────────────────────────────────────────────────── */

    ~Restaurante() override;

    /* ── Overrides de NegocioOO ──────────────────────────────────────────── */

    void mostrar() const override;

    std::string getTipo() const override;

    /* ── Getters propios ─────────────────────────────────────────────────── */

    const std::string& getTipoCocina() const;

    /* ── Setters propios ─────────────────────────────────────────────────── */

    void setTipoCocina(const std::string& tipoCocina);

private:

    std::string _tipoCocina;
};

#endif /* RESTAURANTE_H_ */
