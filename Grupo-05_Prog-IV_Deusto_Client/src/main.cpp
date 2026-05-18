#include "SocketClient.h"
#include "Sesion.h"
#include "Cache.h"
#include "Protocol.h"
#include "menu_bienvenida.h"

#include <iostream>
#include <string>
#include <cstring>

/* ── Configuracion del servidor ─────────────────────────────────────────── */

static const char* SERVER_IP   = "127.0.0.1";
static const int   SERVER_PORT = 6000;

/* ── Modo test ───────────────────────────────────────────────────────────── */


static int runTestMode(const std::string& ip, int puerto)
{
    std::cout << "========================================\n"
              << "  CityHub Client -- MODO TEST\n"
              << "========================================\n"
              << "Servidor: " << ip << ":" << puerto << "\n\n";

    int  passed = 0;
    int  failed = 0;
    bool ok;

    SocketClient sock;

    /* ── Test 1: Conexion TCP ─────────────────────────────────────────── */

    std::cout << "[TEST 1] Conexion TCP... ";

    ok = sock.conectar(ip, puerto);

    if (ok)
    {
        std::cout << "OK\n";
        passed++;
    }
    else
    {
        std::cout << "FALLO\n"
                  << "  -> No se pudo conectar. Comprueba que el servidor este activo.\n";
        failed++;
        goto resultado;   /* Sin conexion no tiene sentido seguir */
    }

    /* ── Test 2: PING / PONG ─────────────────────────────────────────── */

    std::cout << "[TEST 2] PING -> PONG... ";

    ok = sock.enviar(CMD_PING);

    if (!ok)
    {
        std::cout << "FALLO (envio fallido)\n";
        failed++;
        goto desconectar;
    }

    {
        std::string respPing = sock.recibir();

        if (respPing == RES_PONG)
        {
            std::cout << "OK\n";
            passed++;
        }
        else
        {
            std::cout << "FALLO\n"
                      << "  -> Respuesta recibida: \"" << respPing << "\"\n"
                      << "  -> Se esperaba:        \"" << RES_PONG << "\"\n";
            failed++;
        }
    }

    /* ── Test 3: GET_SERVICIOS (protocolo de lista) ───────────────────── */

    std::cout << "[TEST 3] GET_SERVICIOS (protocolo lista)... ";

    ok = sock.enviar(CMD_GET_SERVICIOS) && sock.enviar(buildGetServicios());

    if (!ok)
    {
        std::cout << "FALLO (envio fallido)\n";
        failed++;
        goto desconectar;
    }

    {
        std::vector<std::string> lineas = recibirLista(sock);

        /* La respuesta valida empieza con LIST_START y termina con LIST_END */
        bool listaOk = !lineas.empty()
                       && lineas.front() == RES_LIST_START
                       && lineas.back()  == RES_LIST_END;

        if (listaOk)
        {
            int nServicios = (int)lineas.size() - 2;   /* sin START ni END */
            std::cout << "OK (" << nServicios << " servicio(s) recibido(s))\n";
            passed++;
        }
        else if (lineas.size() == 1 && esError(lineas[0]))
        {
            /* El servidor devolvio un ERR: no es un fallo del cliente */
            std::cout << "OK (servidor respondio ERR: " << lineas[0] << ")\n";
            passed++;
        }
        else
        {
            std::cout << "FALLO\n"
                      << "  -> Respuesta inesperada: \""
                      << (lineas.empty() ? "<vacio>" : lineas[0]) << "\"\n";
            failed++;
        }
    }

desconectar:
    sock.desconectar();

resultado:
    std::cout << "\n----------------------------------------\n"
              << "  Resultado: " << passed << " OK  |  " << failed << " FALLO(S)\n"
              << "----------------------------------------\n";

    return (failed == 0) ? 0 : 1;
}

/* ── main ───────────────────────────────────────────────────────────────── */

int main(int argc, char* argv[])
{
    /* ── Detectar modo test ───────────────────────────────────────────── */

    bool modoTest = false;
    std::string testIp   = SERVER_IP;
    int         testPort = SERVER_PORT;

    for (int i = 1; i < argc; i++)
    {
        if (std::strcmp(argv[i], "--test") == 0)
        {
            modoTest = true;
            /* Los dos argumentos opcionales siguientes son IP y puerto */
            if (i + 1 < argc) { testIp   = argv[++i]; }
            if (i + 1 < argc) { testPort = std::atoi(argv[++i]); }
            break;
        }
    }

    if (modoTest)
    {
        return runTestMode(testIp, testPort);
    }

    /* ── Modo normal ─────────────────────────────────────────────────── */

    SocketClient sock;
    SesionOO     sesion;
    CacheOO      cache;

    std::cout << "Conectando con el servidor " << SERVER_IP
              << ":" << SERVER_PORT << "...\n";

    if (!sock.conectar(SERVER_IP, SERVER_PORT))
    {
        std::cerr << "Error: no se pudo conectar al servidor.\n"
                  << "Comprueba que el servidor este en ejecucion y vuelve a intentarlo.\n";
        return 1;
    }

    std::cout << "Conexion establecida.\n\n";

    gestionMenuBienvenida(sock, sesion, cache);

    cache.limpiarTodo();

    if (sock.estaConectado())
    {
        sock.desconectar();
    }

    std::cout << "\nHasta pronto.\n";

    return 0;
}
