/*
 * config.c
 *
 *  Created on: 7 abr 2026
 *      Author: markel.trigo
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"


static void config_defaults(Config *cfg) {
    strncpy(cfg->db_path,        "bd.db",      sizeof(cfg->db_path) - 1);
    strncpy(cfg->admin_dni,      "admin",  sizeof(cfg->admin_dni) - 1);
    strncpy(cfg->admin_password, "admin",   sizeof(cfg->admin_password) - 1);
    strncpy(cfg->log_path,       "cityhub.log",sizeof(cfg->log_path) - 1);
    cfg->max_negocios = 100;
}

int config_cargar(Config *cfg) {
    if (cfg == NULL) return -1;
    config_defaults(cfg);

    FILE *f = fopen(CONFIG_PATH, "r");
    if (f == NULL) {
        printf("[config] Fichero '%s' no encontrado, usando valores por defecto.\n", CONFIG_PATH);
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
        else if (strcmp(clave, "max_negocios")    == 0) cfg->max_negocios = atoi(valor);
    }

    fclose(f);
    return 0;
}

int config_guardar(const Config *cfg) {
    if (cfg == NULL) return -1;

    FILE *f = fopen(CONFIG_PATH, "w");
    if (f == NULL) {
        printf("[config] Error: no se pudo escribir '%s'.\n", CONFIG_PATH);
        return -1;
    }

    fprintf(f, "# CityHub - fichero de configuracion\n");
    fprintf(f, "# Edita este fichero o usa el menu de configuracion del programa.\n\n");
    fprintf(f, "db_path=%s\n",        cfg->db_path);
    fprintf(f, "admin_dni=%s\n",      cfg->admin_dni);
    fprintf(f, "admin_password=%s\n", cfg->admin_password);
    fprintf(f, "log_path=%s\n",       cfg->log_path);
    fprintf(f, "max_negocios=%d\n",   cfg->max_negocios);

    fclose(f);
    printf("[config] Configuración guardada en '%s'.\n", CONFIG_PATH);
    return 0;
}

void config_mostrar(const Config *cfg) {
    printf("======================= \n");
    printf("Configuracion actual \n");
    printf("Base de datos : %s\n", cfg->db_path);
    printf("Admin DNI     : %s\n", cfg->admin_dni);
    printf("Admin password: %s\n", cfg->admin_password);
    printf("Log path      : %s\n", cfg->log_path);
    printf("Max negocios  : %d\n", cfg->max_negocios);
    printf("======================= \n");
}
