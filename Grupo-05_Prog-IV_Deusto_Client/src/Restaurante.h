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
                const std::string& tipoCocina,
                const std::string& menuDelDia = "");

    /* ── Destructor ──────────────────────────────────────────────────────── */

    ~Restaurante() override;

    /* ── Overrides de NegocioOO ──────────────────────────────────────────── */

    void mostrar() const override;

    std::string getTipo() const override;

    /* ── Getters propios ─────────────────────────────────────────────────── */

    const std::string& getTipoCocina() const;
    const std::string& getMenuDelDia() const;

    /* ── Setters propios ─────────────────────────────────────────────────── */

    void setTipoCocina(const std::string& tipoCocina);
    void setMenuDelDia(const std::string& menuDelDia);

private:

    std::string _tipoCocina;
    std::string _menuDelDia;
};

#endif /* RESTAURANTE_H_ */
