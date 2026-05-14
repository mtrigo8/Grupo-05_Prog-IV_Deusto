#ifndef NEGOCIOFACTORY_H_
#define NEGOCIOFACTORY_H_

#include "Negocio.h"
#include "Protocol.h"

class NegocioFactory
{
public:

    static NegocioOO* crear(const ParsedServicio& parsed, int capacidad = 0);
    static NegocioOO* crearDesdeLinea(const std::string& linea,
                                      int                capacidad = 0);

private:

    /* Clase de utilidad: no se instancia */
    NegocioFactory();
    ~NegocioFactory();
};

#endif /* NEGOCIOFACTORY_H_ */
