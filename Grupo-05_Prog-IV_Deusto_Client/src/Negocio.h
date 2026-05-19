/*
 * Negocio.h
 *
 *  Clase abstracta base NegocioOO + enum TipoNegocio.
 */

#ifndef NEGOCIO_H_
#define NEGOCIO_H_

#include <string>
#include <cctype>    /* tolower */

enum TipoNegocio
{
    TIPO_RESTAURANTE = 0,
    TIPO_SERVICIO    = 1,
    TIPO_ACTIVIDAD   = 2,
    TIPO_DESCONOCIDO = 99
};

inline TipoNegocio tipoDesdeString(const std::string& tipo)
{
    /* Convertir a minusculas para que funcione tanto si el servidor
     * envia "Restaurante" como "restaurante" o "RESTAURANTE" */
    std::string lower = tipo;
    for (size_t i = 0; i < lower.size(); i++)
    {
        lower[i] = static_cast<char>(tolower(static_cast<unsigned char>(lower[i])));
    }

    if (lower == "restaurante") { return TIPO_RESTAURANTE; }
    if (lower == "servicio")    { return TIPO_SERVICIO;    }
    if (lower == "actividad")   { return TIPO_ACTIVIDAD;   }
    return TIPO_DESCONOCIDO;
}


inline std::string tipoAString(TipoNegocio tipo)
{
    if (tipo == TIPO_RESTAURANTE) { return "restaurante"; }
    if (tipo == TIPO_SERVICIO)    { return "servicio";    }
    if (tipo == TIPO_ACTIVIDAD)   { return "actividad";   }
    return "desconocido";
}

/* =========================================================================
 * CLASE ABSTRACTA NegocioOO
 * ========================================================================= */

class NegocioOO
{
public:

    /* ── Constructor ─────────────────────────────────────────────────────── */

    NegocioOO(int                id,
              const std::string& nombre,
              const std::string& municipio,
              const std::string& horaApertura,
              const std::string& horaCierre,
              int                capacidad);

    /* ── Destructor virtual ──────────────────────────────────────────────── */

    virtual ~NegocioOO();

    /* ── Metodos virtuales puros ─────────────────────────────────────────── */

    virtual void        mostrar() const = 0;
    virtual std::string getTipo() const = 0;

    TipoNegocio getTipoEnum() const;

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

    /* ── Helper protegido ────────────────────────────────────────────────── */

    void mostrarComun() const;
};

#endif /* NEGOCIO_H_ */
