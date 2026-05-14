#ifndef SESION_H_
#define SESION_H_

#include <string>

class SesionOO
{
public:

    /* ── Constructor ─────────────────────────────────────────────────────── */

    SesionOO();

    /* ── Destructor ──────────────────────────────────────────────────────── */

    ~SesionOO();

    /* ── Gestion de sesion ───────────────────────────────────────────────── */
    void iniciarSesion(int                idUsuario,
                       const std::string& nombre,
                       const std::string& rol);

    void cerrarSesion();

    /* ── Consultas de estado ─────────────────────────────────────────────── */
    bool estaAutenticado() const;

    /* ── Getters ─────────────────────────────────────────────────────────── */
    int getId() const;
    const std::string& getNombre() const;
    const std::string& getRol() const;

private:

    int         _idUsuario;
    std::string _nombre;
    std::string _rol;
    bool        _autenticado;
};

#endif /* SESION_H_ */
