
#ifndef NEGOCIO_H_
#define NEGOCIO_H_

#include <string>

class NegocioOO
{
public:

    NegocioOO(int                id,
              const std::string& nombre,
              const std::string& municipio,
              const std::string& horaApertura,
              const std::string& horaCierre,
              int                capacidad);

    /* ── Destructor virtual ──────────────────────────────────────────────── */
    virtual ~NegocioOO();

    /* ── Metodos virtuales puros ─────────────────────────────────────────── */
    virtual void mostrar() const = 0;

    virtual std::string getTipo() const = 0;

    /* ── Getters ─────────────────────────────────────────────────────────── */

    int                getId()           const;
    const std::string& getNombre()       const;
    const std::string& getMunicipio()    const;
    const std::string& getHoraApertura() const;
    const std::string& getHoraCierre()   const;
    int                getCapacidad()    const;

    /* ── Setters ─────────────────────────────────────────────────────────── */

    void setCapacidad(int capacidad);
    void setHoraApertura(const std::string& horaApertura);
    void setHoraCierre(const std::string& horaCierre);

protected:

    /* ── Atributos comunes (accesibles desde subclases) ─────────────────── */

    int         _id;
    std::string _nombre;
    std::string _municipio;
    std::string _horaApertura;
    std::string _horaCierre;
    int         _capacidad;

    void mostrarComun() const;
};

#endif /* NEGOCIO_H_ */
