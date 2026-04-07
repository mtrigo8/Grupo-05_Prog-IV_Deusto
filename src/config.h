/*
 * config.h
 *
 *  Created on: 7 abr 2026
 *      Author: markel.trigo
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define CONFIG_PATH "cityhub.conf"

typedef struct {
    char db_path[256];
    char admin_dni[20];
    char admin_password[50];
    char log_path[256];
    int  max_negocios;
} Config;


int  config_cargar(Config *cfg);


int  config_guardar(const Config *cfg);


void config_mostrar(const Config *cfg);

#endif
