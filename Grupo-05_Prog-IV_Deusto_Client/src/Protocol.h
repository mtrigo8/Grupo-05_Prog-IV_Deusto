#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <string>
#include <vector>

/* =========================================================================
 * CONFIGURACION DE RED
 * ========================================================================= */

static const int BUFF_SIZE = 1024;   /* Debe coincidir con el servidor       */

/* =========================================================================
 * SEPARADOR
 * ========================================================================= */

static const char SEP_CHAR = '|';
static const std::string SEP = "|";

/* =========================================================================
 * COMANDOS  (el cliente los envia, el servidor los reconoce con strcmp)
 * ========================================================================= */

/* -- Autenticacion -------------------------------------------------------- */
static const std::string CMD_LOGIN    = "LOGIN";    /* params: dni|pass_hash              */
static const std::string CMD_REGISTER = "REGISTER"; /* params: nombre|apellido|dni|hash   */
static const std::string CMD_LOGOUT   = "LOGOUT";   /* params: id_usuario                 */

/* -- Servicios ------------------------------------------------------------ */
static const std::string CMD_GET_SERVICIOS    = "GET_SERVICIOS";    /* params: tipo       */
static const std::string CMD_GET_SERVICIO     = "GET_SERVICIO";     /* params: id         */
static const std::string CMD_FILTER_SERVICIOS = "FILTER_SERVICIOS"; /* params: tipo|fecha */

/* -- Reservas ------------------------------------------------------------- */
static const std::string CMD_CREATE_RESERVA = "CREATE_RESERVA"; /* params: id_u|id_s      */
static const std::string CMD_GET_RESERVA    = "GET_RESERVA";    /* params: id_usuario     */
static const std::string CMD_CANCEL_RESERVA = "CANCEL_RESERVA"; /* params: id_r|id_u      */
static const std::string CMD_UPDATE_RESERVA = "UPDATE_RESERVA"; /* params: id_r|id_u|id_s */

/* -- Conexion ------------------------------------------------------------- */
static const std::string CMD_PING       = "PING";       /* sin parametros              */
static const std::string CMD_DISCONNECT = "DISCONNECT"; /* params: id_usuario          */

/* =========================================================================
 * RESPUESTAS DEL SERVIDOR
 * ========================================================================= */

/* -- Exito ---------------------------------------------------------------- */
static const std::string RES_OK             = "OK";
static const std::string RES_PONG           = "PONG";
static const std::string RES_SESION_CERRADA = "OK|SESION_CERRADA";
static const std::string RES_CANCELADA      = "OK|CANCELADA";
static const std::string RES_DESCONECTADO   = "OK|DESCONECTADO";

/* -- Error ---------------------------------------------------------------- */
static const std::string RES_ERR_CREDENCIALES  = "ERR|CREDENCIALES_INVALIDAS";
static const std::string RES_ERR_DNI_DUP       = "ERR|DNI_DUPLICADO";
static const std::string RES_ERR_NO_ENCONTRADO = "ERR|NO_ENCONTRADO";
static const std::string RES_ERR_SIN_CUPOS     = "ERR|SIN_CUPOS";
static const std::string RES_ERR_YA_RESERVADO  = "ERR|YA_RESERVADO";
static const std::string RES_ERR_NO_CANCELABLE = "ERR|NO_CANCELABLE";
static const std::string RES_ERR_NO_AUTORIZADO = "ERR|NO_AUTORIZADO";
static const std::string RES_ERR_GENERICO      = "ERR|ERROR";

/* -- Lista ---------------------------------------------------------------- */
static const std::string RES_LIST_START = "LIST_START";
static const std::string RES_LIST_END   = "LIST_END";


inline std::vector<std::string> splitSEP(const std::string& s)
{
    std::vector<std::string> campos;
    std::string campo;

    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == SEP_CHAR)
        {
            campos.push_back(campo);
            campo.clear();
        }
        else
        {
            campo += s[i];
        }
    }
    campos.push_back(campo);   /* ultimo campo (sin '|' al final) */

    return campos;
}

inline bool esError(const std::string& respuesta)
{
    return respuesta.size() >= 3 && respuesta.substr(0, 3) == "ERR";
}

inline bool esOk(const std::string& respuesta)
{
    return respuesta.size() >= 2 && respuesta.substr(0, 2) == "OK";
}

/* -- Autenticacion -------------------------------------------------------- */

/*
 * buildLogin
 *  Construye: "dni|password_hash"
 */
inline std::string buildLogin(const std::string& dni,
                              const std::string& passwordHash)
{
    return dni + SEP + passwordHash;
}

/*
 * buildRegister
 *  Construye: "nombre|apellido|dni|password_hash"
 */
inline std::string buildRegister(const std::string& nombre,
                                 const std::string& apellido,
                                 const std::string& dni,
                                 const std::string& passwordHash)
{
    return nombre + SEP + apellido + SEP + dni + SEP + passwordHash;
}

/*
 * buildLogout
 *  Construye: "id_usuario"
 */
inline std::string buildLogout(int idUsuario)
{
    return std::to_string(idUsuario);
}

/* -- Servicios ------------------------------------------------------------ */

/*
 * buildGetServicios
 *  Construye: "tipo"  (cadena vacia o "TODOS" para todos los tipos)
 */
inline std::string buildGetServicios(const std::string& tipo = "TODOS")
{
    return tipo;
}

/*
 * buildGetServicio
 *  Construye: "id_servicio"
 */
inline std::string buildGetServicio(int idServicio)
{
    return std::to_string(idServicio);
}

/*
 * buildFilterServicios
 *  Construye: "tipo|fecha_mascara"
 *  fecha_mascara: entero de bits de dias (0 = todos los dias)
 */
inline std::string buildFilterServicios(const std::string& tipo,
                                        int fechaMascara = 0)
{
    return tipo + SEP + std::to_string(fechaMascara);
}

/* -- Reservas ------------------------------------------------------------- */

/*
 * buildCreateReserva
 *  Construye: "id_usuario|id_servicio|fecha"
 *  fecha formato: yyyy-mm-dd
 */
inline std::string buildCreateReserva(int idUsuario, int idServicio,
                                      const std::string& fecha)
{
    return std::to_string(idUsuario) + SEP +
           std::to_string(idServicio) + SEP +
           fecha;
}

/*
 * buildGetReserva
 *  Construye: "id_usuario"
 */
inline std::string buildGetReserva(int idUsuario)
{
    return std::to_string(idUsuario);
}

/*
 * buildCancelReserva
 *  Construye: "id_reserva|id_usuario"
 */
inline std::string buildCancelReserva(int idReserva, int idUsuario)
{
    return std::to_string(idReserva) + SEP + std::to_string(idUsuario);
}

/*
 * buildUpdateReserva
 *  Construye: "id_reserva|id_usuario|id_nuevo_servicio|fecha"
 *  fecha formato: yyyy-mm-dd
 */
inline std::string buildUpdateReserva(int idReserva,
                                      int idUsuario,
                                      int idNuevoServicio,
                                      const std::string& fecha)
{
    return std::to_string(idReserva) + SEP +
           std::to_string(idUsuario) + SEP +
           std::to_string(idNuevoServicio) + SEP +
           fecha;
}

/* -- Conexion ------------------------------------------------------------- */

/*
 * buildDisconnect
 *  Construye: "id_usuario"
 */
inline std::string buildDisconnect(int idUsuario)
{
    return std::to_string(idUsuario);
}

/* =========================================================================
 * STRUCTS de respuesta  —  resultado de cada parse*
 * ========================================================================= */

/* Resultado de parseLogin / parseRegister */
struct ParsedAuth
{
    bool        ok;          /* true si el servidor respondio OK            */
    int         idUsuario;   /* id asignado por el servidor (0 si error)    */
    std::string nombre;      /* nombre del usuario (vacio si error)         */
    std::string rol;         /* "cliente" u otro rol (vacio si error)       */
    std::string errorMsg;    /* campo de error si ok == false               */
};

/* Resultado de parseServicio (una linea de la lista) */
struct ParsedServicio
{
    int         id;           /* rowid en BD (presente en GET_SERVICIO/FILTER) */
    std::string nombre;
    std::string municipio;
    std::string horaApertura;
    std::string horaCierre;
    std::string dias;
    std::string tipo;
    int         capacidad;    /* capacidad_maxima de la BD                     */
    std::string descripcion;  /* campo especial: tipoCocina / descripcion / -  */
    int         ocupadas;     /* reservas actuales (subquery en servidor)       */
};

/* Resultado de parseReserva (una linea de la lista) */
struct ParsedReserva
{
    int         idReserva;
    int         idServicio;
    std::string nombreServicio;
    std::string fecha;
    std::string estado;
};

/* =========================================================================
 * PARSERS  —  descomponen la respuesta del servidor en structs legibles
 * ========================================================================= */

/* -- Autenticacion -------------------------------------------------------- */
inline ParsedAuth parseLogin(const std::string& respuesta)
{
    ParsedAuth resultado;
    resultado.ok        = false;
    resultado.idUsuario = 0;

    std::vector<std::string> campos = splitSEP(respuesta);

    if (campos.size() >= 1 && campos[0] == "OK")
    {
        resultado.ok = true;

        if (campos.size() >= 2) { resultado.idUsuario = std::stoi(campos[1]); }
        if (campos.size() >= 3) { resultado.nombre    = campos[2];            }
        if (campos.size() >= 4) { resultado.rol       = campos[3];            }
    }
    else
    {
        resultado.ok = false;

        if (campos.size() >= 2) { resultado.errorMsg = campos[1]; }
        else                    { resultado.errorMsg = respuesta;  }
    }

    return resultado;
}

inline ParsedAuth parseRegister(const std::string& respuesta)
{
    ParsedAuth resultado;
    resultado.ok        = false;
    resultado.idUsuario = 0;

    std::vector<std::string> campos = splitSEP(respuesta);

    if (campos.size() >= 1 && campos[0] == "OK")
    {
        resultado.ok = true;

        if (campos.size() >= 2) { resultado.idUsuario = std::stoi(campos[1]); }
    }
    else
    {
        resultado.ok = false;

        if (campos.size() >= 2) { resultado.errorMsg = campos[1]; }
        else                    { resultado.errorMsg = respuesta;  }
    }

    return resultado;
}

/* -- Servicios ------------------------------------------------------------ */


inline ParsedServicio parseServicio(const std::string& linea)
{
    ParsedServicio s;
    s.id        = 0;
    s.capacidad = 0;

    std::vector<std::string> campos = splitSEP(linea);

    if (campos.empty())
    {
        return s;
    }

    /* Detectar si el primer campo es un id numerico */
    bool tieneId = true;
    for (size_t i = 0; i < campos[0].size(); i++)
    {
        if (campos[0][i] < '0' || campos[0][i] > '9')
        {
            tieneId = false;
            break;
        }
    }

    /* Protocolo con id:
     *  [0]id | [1]nombre | [2]municipio | [3]horaApertura | [4]horaCierre
     *  [5]dias | [6]tipo | [7]capacidad | [8]descripcion
     */
    if (tieneId && campos.size() >= 7)
    {
        s.id           = std::stoi(campos[0]);
        s.nombre       = campos[1];
        s.municipio    = campos[2];
        s.horaApertura = campos[3];
        s.horaCierre   = campos[4];
        s.dias         = campos[5];
        s.tipo         = campos[6];
        if (campos.size() >= 8) { s.capacidad   = std::stoi(campos[7]); }
        if (campos.size() >= 9) { s.descripcion = campos[8];             }
    }
    else if (!tieneId && campos.size() >= 6)
    {
        s.nombre       = campos[0];
        s.municipio    = campos[1];
        s.horaApertura = campos[2];
        s.horaCierre   = campos[3];
        s.dias         = campos[4];
        s.tipo         = campos[5];
        if (campos.size() >= 7) { s.capacidad   = std::stoi(campos[6]); }
        if (campos.size() >= 8) { s.descripcion = campos[7];             }
    }

    return s;
}

inline std::vector<ParsedServicio> parseListaServicios(
    const std::vector<std::string>& lineas)
{
    std::vector<ParsedServicio> lista;

    for (size_t i = 0; i < lineas.size(); i++)
    {
        if (lineas[i] == RES_LIST_START) { continue; }
        if (lineas[i] == RES_LIST_END)   { break;    }

        lista.push_back(parseServicio(lineas[i]));
    }

    return lista;
}

/* -- Reservas ------------------------------------------------------------- */


inline ParsedReserva parseReserva(const std::string& linea)
{
    ParsedReserva r;
    r.idReserva  = 0;
    r.idServicio = 0;

    std::vector<std::string> campos = splitSEP(linea);

    if (campos.size() >= 1) { r.idReserva      = std::stoi(campos[0]); }
    if (campos.size() >= 2) { r.idServicio      = std::stoi(campos[1]); }
    if (campos.size() >= 3) { r.nombreServicio  = campos[2];            }
    if (campos.size() >= 4) { r.fecha           = campos[3];            }
    if (campos.size() >= 5) { r.estado          = campos[4];            }

    return r;
}

/*
 * parseListaReservas
 *
 *  Equivalente a parseListaServicios para reservas.
 */
inline std::vector<ParsedReserva> parseListaReservas(
    const std::vector<std::string>& lineas)
{
    std::vector<ParsedReserva> lista;

    for (size_t i = 0; i < lineas.size(); i++)
    {
        if (lineas[i] == RES_LIST_START) { continue; }
        if (lineas[i] == RES_LIST_END)   { break;    }

        lista.push_back(parseReserva(lineas[i]));
    }

    return lista;
}


template<typename TSocket>
std::vector<std::string> recibirLista(TSocket& socket)
{
    std::vector<std::string> lineas;
    std::string linea;

    /* Primera recepcion: esperamos LIST_START o un ERR */
    linea = socket.recibir();

    if (linea != RES_LIST_START)
    {
        /* El servidor devolvio un error en lugar de la lista */
        lineas.push_back(linea);
        return lineas;
    }

    lineas.push_back(linea);

    /* Seguir recibiendo hasta LIST_END */
    while (true)
    {
        linea = socket.recibir();
        lineas.push_back(linea);

        if (linea == RES_LIST_END) { break; }
        if (linea.empty())         { break; } /* conexion cerrada */
    }

    return lineas;
}

#endif /* PROTOCOL_H_ */
