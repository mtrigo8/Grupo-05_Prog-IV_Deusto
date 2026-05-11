
#ifndef SOCKETCLIENT_H_
#define SOCKETCLIENT_H_

#include <winsock2.h>
#include <ws2tcpip.h>

#include <string>

class SocketClient {
public:
    /* ── Constructor / Destructor ── */
    SocketClient();
    ~SocketClient();

    /*
     * Establece la conexión TCP con el servidor.
     * @param ip     Dirección IP del servidor (ej. "127.0.0.1")
     * @param puerto Puerto del servidor (ej. 8080)
     * @return true si la conexión fue exitosa, false en caso contrario
     */
    bool conectar(const std::string& ip, int puerto);

    /*
     * Cierra la conexión con el servidor si está abierta.
     */
    void desconectar();

    /*
     * Envía un mensaje al servidor.
     * @param mensaje Cadena a enviar
     * @return true si el envío fue exitoso, false en caso contrario
     */
    bool enviar(const std::string& mensaje);

    /*
     * Recibe un mensaje del servidor (bloqueante).
     * @return Cadena recibida, o "" si hubo error o la conexión se cerró
     */
    std::string recibir();

    /*
     * Indica si el socket está actualmente conectado.
     */
    bool estaConectado() const;

private:
    SOCKET _sockfd;    // Descriptor del socket (INVALID_SOCKET si no conectado)
    bool   _conectado; // Estado de la conexión
};

#endif /* SOCKETCLIENT_H_ */
