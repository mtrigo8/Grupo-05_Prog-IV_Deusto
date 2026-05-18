#ifndef CACHE_H_
#define CACHE_H_

#include "Negocio.h"
#include "Reserva.h"

#include <vector>
#include <string>

class CacheOO
{
public:

    /* ── Constructor / Destructor ────────────────────────────────────────── */

    CacheOO();

    ~CacheOO();

    /* =====================================================================
     * NEGOCIOS
     * ===================================================================== */


    void agregarNegocio(NegocioOO* negocio);
    void limpiarNegocios();
    int getTotalNegocios() const;
    NegocioOO* getNegocio(int indice) const;
    NegocioOO* buscarNegocioPorId(int id) const;
    void mostrarNegocios() const;
    std::vector<NegocioOO*>& getNegocios() const;
    void mostrarNegociosFiltrados(const std::string& tipo) const;

    /* =====================================================================
     * RESERVAS
     * ===================================================================== */
    void agregarReserva(Reserva* reserva);
    void limpiarReservas();
    int getTotalReservas() const;
    Reserva* getReserva(int indice) const;
    Reserva* buscarReservaPorId(int idReserva) const;
    void mostrarReservas() const;

    /* =====================================================================
     * UTILIDAD GENERAL
     * ===================================================================== */
    void limpiarTodo();

private:

    std::vector<NegocioOO*> _negocios;
    std::vector<Reserva*>   _reservas;

    CacheOO(const CacheOO&);
    CacheOO& operator=(const CacheOO&);
};

#endif /* CACHE_H_ */
