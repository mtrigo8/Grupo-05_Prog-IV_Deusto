#ifndef RESERVA_H_
#define RESERVA_H_

#include <string>

class Reserva
{
public:

    /* ── Constructor ─────────────────────────────────────────────────────── */

    Reserva(int                idReserva,
            int                idUsuario,
            int                idServicio,
            const std::string& fechaRegistro,
            const std::string& nombreServicio,
            const std::string& estado = "ACTIVA");

    /* ── Destructor ──────────────────────────────────────────────────────── */

    ~Reserva();

    /* ── Metodo de presentacion ──────────────────────────────────────────── */

    void mostrar() const;

    /* ── Getters ─────────────────────────────────────────────────────────── */

    int                getIdReserva()      const;
    int                getIdUsuario()      const;
    int                getIdServicio()     const;
    const std::string& getFechaRegistro()  const;
    const std::string& getNombreServicio() const;
    const std::string& getEstado()         const;

    /* ── Setters ─────────────────────────────────────────────────────────── */

    void setEstado(const std::string& estado);

    /* ── Helper de estado ────────────────────────────────────────────────── */

    bool estaActiva() const;

private:

    int         _idReserva;
    int         _idUsuario;
    int         _idServicio;
    std::string _fechaRegistro;
    std::string _nombreServicio;
    std::string _estado;
};

#endif /* RESERVA_H_ */
