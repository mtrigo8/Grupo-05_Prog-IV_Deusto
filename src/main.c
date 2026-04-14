#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "logicaMenus.h"
#include "menus.h"
#include "config.h"
#include "db.h"

int main(void) {
    Config cfg;
    config_cargar(&cfg);
    log_init(cfg.log_path);
    sqlite3 *db;
    int rc = sqlite3_open_v2(cfg.db_path, &db, SQLITE_OPEN_READWRITE, NULL);
    if (rc != SQLITE_OK) {
        printf("No se pudo abrir la base de datos: %s\n", cfg.db_path);
        return 1;
    }
    sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
    gestionMenuBienvenida(db, &cfg);

    sqlite3_close(db);
	return EXIT_SUCCESS;
}
