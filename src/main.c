#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "logicaMenus.h"
#include "menus.h"

int main(void) {
	sqlite3 *db;
	int rc = sqlite3_open_v2("bd.db", &db, SQLITE_OPEN_READWRITE, NULL);
	//int rc = sqlite3_open("bd.db", &db); // Asegúrate de que el nombre sea exacto

	if (rc != SQLITE_OK) {
        printf("No se pudo abrir la base de datos\n");
        return 1;
    }

    gestionMenuBienvenida(db);

    sqlite3_close(db);
	return EXIT_SUCCESS;
}
