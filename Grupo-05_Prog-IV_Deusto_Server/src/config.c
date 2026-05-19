/*
 * config.c
 */

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hash.h"

static void config_defaults(Config *cfg) {
    memset(cfg, 0, sizeof(Config));
    char hash[65];
    sha256_hex("admin", hash);
    strncpy(cfg->db_path,        "bd.db",        sizeof(cfg->db_path) - 1);
    strncpy(cfg->admin_dni,      hash,            sizeof(cfg->admin_dni) - 1);
    strncpy(cfg->admin_password, hash,            sizeof(cfg->admin_password) - 1);
    strncpy(cfg->log_path,       "cityhub.log",   sizeof(cfg->log_path) - 1);
    strncpy(cfg->server_log_path,"server.log",    sizeof(cfg->server_log_path) - 1);
    strncpy(cfg->server_ip,      "127.0.0.1",     sizeof(cfg->server_ip) - 1);
    cfg->max_negocios = 100;

    /*
     * BUG FIX: el puerto por defecto era 6000 pero el cliente usa 8080.
     * Ahora ambos usan 8080 como valor por defecto.
     * Si tienes un server.config con "puerto=XXXX" ese valor tiene prioridad.
     */
    cfg->server_port  = 8080;
}

int config_cargar(Config *cfg) {
    if (cfg == NULL) return -1;
    config_defaults(cfg);

    FILE *f = fopen(CONFIG_PATH, "r");
    if (f == NULL) {
        printf("[config] Fichero '%s' no encontrado, usando valores por defecto.\n", CONFIG_PATH);
        fflush(stdout);
        return -1;
    }

    char linea[512];
    while (fgets(linea, sizeof(linea), f)) {
        if (linea[0] == '#' || linea[0] == '\n' || linea[0] == '\r')
            continue;

        char clave[128], valor[384];
        if (sscanf(linea, " %127[^=]=%383[^\n]", clave, valor) != 2)
            continue;

        int len = (int)strlen(valor);
        while (len > 0 && (valor[len-1] == ' ' || valor[len-1] == '\r'))
            valor[--len] = '\0';

        if      (strcmp(clave, "db_path")        == 0) strncpy(cfg->db_path,        valor, sizeof(cfg->db_path) - 1);
        else if (strcmp(clave, "admin_dni")       == 0) strncpy(cfg->admin_dni,       valor, sizeof(cfg->admin_dni) - 1);
        else if (strcmp(clave, "admin_password")  == 0) strncpy(cfg->admin_password,  valor, sizeof(cfg->admin_password) - 1);
        else if (strcmp(clave, "log_path")        == 0) strncpy(cfg->log_path,        valor, sizeof(cfg->log_path) - 1);
        else if (strcmp(clave, "server_log_path") == 0) strncpy(cfg->server_log_path, valor, sizeof(cfg->server_log_path) - 1);
        else if (strcmp(clave, "ip_servidor")     == 0) strncpy(cfg->server_ip,       valor, sizeof(cfg->server_ip) - 1);
        else if (strcmp(clave, "max_negocios")    == 0) cfg->max_negocios = atoi(valor);
        else if (strcmp(clave, "puerto")          == 0) cfg->server_port  = atoi(valor);
    }

    fclose(f);
    return 0;
}

int config_guardar(const Config *cfg) {
    if (cfg == NULL) return -1;

    FILE *f = fopen(CONFIG_PATH, "w");
    if (f == NULL) {
        printf("[config] Error: no se pudo escribir '%s'.\n", CONFIG_PATH);
        fflush(stdout);
        return -1;
    }

    fprintf(f, "# CityHub - fichero de configuracion\n");
    fprintf(f, "# Edita este fichero o usa el menu de configuracion del programa.\n\n");
    fprintf(f, "db_path=%s\n",         cfg->db_path);
    fprintf(f, "admin_dni=%s\n",       cfg->admin_dni);
    fprintf(f, "admin_password=%s\n",  cfg->admin_password);
    fprintf(f, "log_path=%s\n",        cfg->log_path);
    fprintf(f, "max_negocios=%d\n",    cfg->max_negocios);
    fprintf(f, "\n# Servidor TCP\n");
    fprintf(f, "server_log_path=%s\n", cfg->server_log_path);
    fprintf(f, "ip_servidor=%s\n",     cfg->server_ip);
    fprintf(f, "puerto=%d\n",          cfg->server_port);

    fclose(f);
    printf("[config] Configuracion guardada en '%s'.\n", CONFIG_PATH);
    fflush(stdout);
    return 0;
}

void config_mostrar(const Config *cfg) {
    printf("======================= \n");
    printf("Configuracion actual \n");
    printf("Base de datos    : %s\n", cfg->db_path);
    printf("Admin DNI        : %s\n", cfg->admin_dni);
    printf("Admin password   : %s\n", cfg->admin_password);
    printf("Log path (admin) : %s\n", cfg->log_path);
    printf("Max negocios     : %d\n", cfg->max_negocios);
    printf("--- Servidor TCP ---\n");
    printf("Server log path  : %s\n", cfg->server_log_path);
    printf("IP servidor      : %s\n", cfg->server_ip);
    printf("Puerto           : %d\n", cfg->server_port);
    printf("======================= \n");
    fflush(stdout);
}
