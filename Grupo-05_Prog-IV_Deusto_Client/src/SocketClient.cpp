#include "SocketClient.h"

#include <iostream>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

/* Tamaño del frame: debe coincidir con BUFF_SIZE del servidor (protocol.h) */
static const int FRAME_SIZE = 1024;

/* ── Constructor ── */
SocketClient::SocketClient()
    : _sockfd(INVALID_SOCKET), _conectado(false)
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "[SocketClient] Error al inicializar Winsock: "
                  << WSAGetLastError() << "\n";
    }
}

/* ── Destructor ── */
SocketClient::~SocketClient() {
    desconectar();
    WSACleanup();
}

/* ── conectar ── */
bool SocketClient::conectar(const std::string& ip, int puerto) {
    if (_conectado) {
        std::cerr << "[SocketClient] Ya hay una conexion activa.\n";
        return false;
    }

    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == INVALID_SOCKET) {
        std::cerr << "[SocketClient] Error al crear socket: "
                  << WSAGetLastError() << "\n";
        return false;
    }

    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_port        = htons(static_cast<u_short>(puerto));
    serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(_sockfd,
                reinterpret_cast<struct sockaddr*>(&serv_addr),
                sizeof(serv_addr)) == SOCKET_ERROR)
    {
        std::cerr << "[SocketClient] Error al conectar con "
                  << ip << ":" << puerto << " -> "
                  << WSAGetLastError() << "\n";
        closesocket(_sockfd);
        _sockfd = INVALID_SOCKET;
        return false;
    }

    _conectado = true;
    return true;
}

/* ── desconectar ── */
void SocketClient::desconectar() {
    if (_sockfd != INVALID_SOCKET) {
        closesocket(_sockfd);
        _sockfd = INVALID_SOCKET;
    }
    _conectado = false;
}

/* ── enviar ──
 *
 * FIX: La version anterior enviaba exactamente mensaje.size() bytes.
 * Como el servidor hace recv(sock, buf, BUFF_SIZE=1024), TCP puede
 * coalescer dos send() consecutivos (p.ej. "GET_SERVICIOS" + "TODOS")
 * en un unico recv() del servidor, que recibe "GET_SERVICIOSTODOS" y
 * lo trata como comando desconocido.
 *
 * La solucion es enviar siempre un frame de exactamente FRAME_SIZE bytes
 * (igual que recibir() lee exactamente FRAME_SIZE bytes), rellenando con
 * ceros el espacio sobrante. Asi cada recv() del servidor corresponde
 * exactamente a un mensaje y strcmp() funciona correctamente.
 */
bool SocketClient::enviar(const std::string& mensaje) {
    if (!_conectado) {
        std::cerr << "[SocketClient] No hay conexion activa para enviar.\n";
        return false;
    }

    /* Construir frame de tamano fijo relleno de ceros */
    char frame[FRAME_SIZE];
    std::memset(frame, 0, sizeof(frame));

    /* Copiar el mensaje, truncando si supera el frame (no deberia ocurrir) */
    std::size_t copiar = mensaje.size();
    if (copiar >= static_cast<std::size_t>(FRAME_SIZE)) {
        copiar = FRAME_SIZE - 1;   /* dejar al menos un \0 al final */
    }
    std::memcpy(frame, mensaje.c_str(), copiar);

    /* Enviar el frame completo */
    int total   = FRAME_SIZE;
    int enviado = 0;

    while (enviado < total) {
        int n = send(_sockfd,
                     frame + enviado,
                     total - enviado,
                     0);
        if (n == SOCKET_ERROR) {
            std::cerr << "[SocketClient] Error al enviar: "
                      << WSAGetLastError() << "\n";
            desconectar();
            return false;
        }
        enviado += n;
    }

    return true;
}

/* ── recibir ── */
std::string SocketClient::recibir() {
    if (!_conectado) {
        std::cerr << "[SocketClient] No hay conexion activa para recibir.\n";
        return "";
    }

    char buffer[FRAME_SIZE];
    int  leido = 0;

    std::memset(buffer, 0, sizeof(buffer));

    while (leido < FRAME_SIZE)
    {
        int n = recv(_sockfd,
                     buffer + leido,
                     FRAME_SIZE - leido,
                     0);

        if (n == SOCKET_ERROR) {
            std::cerr << "[SocketClient] Error al recibir: "
                      << WSAGetLastError() << "\n";
            desconectar();
            return "";
        }

        if (n == 0) {
            desconectar();
            return "";
        }

        leido += n;
    }

    return std::string(buffer);
}

/* ── estaConectado ── */
bool SocketClient::estaConectado() const {
    return _conectado;
}
