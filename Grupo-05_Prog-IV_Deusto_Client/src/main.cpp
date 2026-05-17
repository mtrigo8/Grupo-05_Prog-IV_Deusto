#include "SocketClient.h"
#include "Sesion.h"
#include "Cache.h"
#include "menu_bienvenida.h"

#include <iostream>

/* ── Configuracion del servidor ─────────────────────────────────────────── */

static const char   *SERVER_IP   = "127.0.0.1";
static const int     SERVER_PORT = 8080;

/* ── main ───────────────────────────────────────────────────────────────── */

int main()
{
    /* ── 1. Crear objetos de infraestructura ─────────────────────────────── */

    SocketClient sock;    /* conexion TCP — no conectado todavia           */
    SesionOO     sesion;  /* usuario no autenticado (_autenticado = false) */
    CacheOO      cache;   /* vectores vacios listos para recibir datos     */

    /* ── 2. Conectar al servidor ─────────────────────────────────────────── */

    std::cout << "Conectando con el servidor " << SERVER_IP
              << ":" << SERVER_PORT << "...\n";

    if (!sock.conectar(SERVER_IP, SERVER_PORT))
    {
        std::cerr << "Error: no se pudo conectar al servidor.\n"
                  << "Comprueba que el servidor este en ejecucion y vuelve a intentarlo.\n";
        return 1;
    }

    std::cout << "Conexion establecida.\n\n";

    /* ── 3. Pasar control al menu de bienvenida ──────────────────────────── */

    gestionMenuBienvenida(sock, sesion, cache);

    /* ── 4. Limpieza y cierre ordenado ───────────────────────────────────── */

    cache.limpiarTodo();

    if (sock.estaConectado())
    {
        sock.desconectar();
    }

    std::cout << "\nHasta pronto.\n";

    return 0;
}
