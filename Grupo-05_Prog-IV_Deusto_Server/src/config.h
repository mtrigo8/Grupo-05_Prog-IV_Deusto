/*
 * config.h
 *
 *  Created on: 7 abr 2026
 *      Author: markel.trigo
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* Nombre del fichero de configuracion del servidor */
#define CONFIG_PATH "server.config"

typedef struct {
    /* ── Comunes (Admin + Servidor) ─────────────────────────────────────── */
    char db_path[256];
    char admin_dni[65];
    char admin_password[65];
    char log_path[256];        /* log del modulo Admin (BD + fichero)        */
    int  max_negocios;

    /* ── Especificos del Servidor TCP ───────────────────────────────────── */
    char server_log_path[256]; /* ruta del log de red del servidor           */
    char server_ip[64];        /* IP en la que escucha el servidor           */
    int  server_port;          /* puerto TCP del servidor                    */
} Config;


int  config_cargar(Config *cfg);

int  config_guardar(const Config *cfg);

void config_mostrar(const Config *cfg);

#endif /* CONFIG_H_ */
