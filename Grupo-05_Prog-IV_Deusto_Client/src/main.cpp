#include "SocketClient.h"
#include "Sesion.h"
#include "Cache.h"
#include "Protocol.h"
#include "menu_bienvenida.h"

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>

/* ── Configuracion por defecto ──────────────────────────────────────────── */

static const char* DEFAULT_IP   = "127.0.0.1";
static const int   DEFAULT_PORT = 8080;

/* ── Uso ────────────────────────────────────────────────────────────────── */

static void mostrarUso(const char* nombreEjecutable)
{
    std::cout << "Uso: " << nombreEjecutable
              << " [ip_servidor] [puerto]\n\n"
              << "  ip_servidor  IP del servidor CityHub (por defecto: "
              << DEFAULT_IP   << ")\n"
              << "  puerto       Puerto TCP del servidor  (por defecto: "
              << DEFAULT_PORT << ")\n\n"
              << "Ejemplos:\n"
              << "  " << nombreEjecutable << "\n"
              << "  " << nombreEjecutable << " 192.168.1.10\n"
              << "  " << nombreEjecutable << " 192.168.1.10 8080\n";
}

/* ── main ───────────────────────────────────────────────────────────────── */

int main(int argc, char* argv[])
{
    /* ── Parsear argumentos opcionales ───────────────────────────────── */

    /* Aceptamos: programa [ip] [puerto]
     * Si el primer argumento es -h o --help mostramos la ayuda. */
    if (argc >= 2 &&
        (std::strcmp(argv[1], "-h") == 0 ||
         std::strcmp(argv[1], "--help") == 0))
    {
        mostrarUso(argv[0]);
        return 0;
    }

    std::string serverIp   = DEFAULT_IP;
    int         serverPort = DEFAULT_PORT;

    if (argc >= 2) { serverIp   = argv[1]; }
    if (argc >= 3) { serverPort = std::atoi(argv[2]); }

    if (serverPort <= 0 || serverPort > 65535)
    {
        std::cerr << "Error: puerto invalido '" << argv[2] << "'.\n";
        mostrarUso(argv[0]);
        return 1;
    }

    /* ── Conectar al servidor ─────────────────────────────────────────── */

    SocketClient sock;
    SesionOO     sesion;
    CacheOO      cache;

    std::cout << "Conectando con el servidor " << serverIp
              << ":" << serverPort << "...\n";

    if (!sock.conectar(serverIp, serverPort))
    {
        std::cerr << "Error: no se pudo conectar al servidor.\n"
                  << "Comprueba que el servidor este en ejecucion y vuelve a intentarlo.\n";
        return 1;
    }

    std::cout << "Conexion establecida.\n\n";

    /* ── Bucle principal de la aplicacion ────────────────────────────── */

    gestionMenuBienvenida(sock, sesion, cache);

    /* ── Limpieza ─────────────────────────────────────────────────────── */

    cache.limpiarTodo();

    if (sock.estaConectado())
    {
        sock.desconectar();
    }

    std::cout << "\nHasta pronto.\n";

    return 0;
}
