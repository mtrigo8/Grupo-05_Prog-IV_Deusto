#include "SocketClient.h"

#include <iostream>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

/* Tamaño del buffer de recepción */
static const int BUFFER_SIZE = 4096;

/* ── Constructor ── */
SocketClient::SocketClient()
    : _sockfd(INVALID_SOCKET), _conectado(false)
{
    // Inicializar Winsock (versión 2.2)
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

    // 1. Crear el socket TCP
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_sockfd == INVALID_SOCKET) {
        std::cerr << "[SocketClient] Error al crear socket: "
                  << WSAGetLastError() << "\n";
        return false;
    }

    // 2. Rellenar la estructura de dirección (igual que el servidor con inet_addr)
    struct sockaddr_in serv_addr;
    std::memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family      = AF_INET;
    serv_addr.sin_port        = htons(static_cast<u_short>(puerto));
    serv_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    // 3. Conectar
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

/* ── enviar ── */
bool SocketClient::enviar(const std::string& mensaje) {
    if (!_conectado) {
        std::cerr << "[SocketClient] No hay conexion activa para enviar.\n";
        return false;
    }

    // Enviamos el mensaje completo (send puede enviar menos bytes de lo pedido)
    int total   = static_cast<int>(mensaje.size());
    int enviado = 0;

    while (enviado < total) {
        int n = send(_sockfd,
                     mensaje.c_str() + enviado,
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

    char buffer[BUFFER_SIZE];
    std::memset(buffer, 0, sizeof(buffer));

    int n = recv(_sockfd, buffer, BUFFER_SIZE - 1, 0);

    if (n == SOCKET_ERROR) {
        std::cerr << "[SocketClient] Error al recibir: "
                  << WSAGetLastError() << "\n";
        desconectar();
        return "";
    }

    if (n == 0) {
        // El servidor cerró la conexión
        desconectar();
        return "";
    }

    /* El servidor envía buffers de tamaño fijo (BUFF_SIZE) rellenados con \0.
     * Construir el string con el constructor de C-string para que se detenga
     * en el primer \0 y las comparaciones de protocolo funcionen correctamente. */
    return std::string(buffer);
}

/* ── estaConectado ── */
bool SocketClient::estaConectado() const {
    return _conectado;
}
