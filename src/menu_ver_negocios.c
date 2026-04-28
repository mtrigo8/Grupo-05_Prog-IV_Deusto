/*
 * menu_ver_negocios.c
 *
 * Vista y logica del menu de visualizacion de negocios.
 */

#include "../server/menu_ver_negocios.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../server/negocio.h"
#include "../server/sqlite3.h"

/* ── Vista ── */
void mostrarNegocios(Negocio *negocios, int cantidad_total) {
    printf("======================= \n");
    printf("Mostrando todos los negocios en la base de datos\n");
    printf("======================= \n");

    for (int i = 0; i < cantidad_total; i++) {
        Negocio n = negocios[i];
        char dias[100];
        memset(dias, 0, sizeof(dias));
        convertirIntDias(n.fecha, dias);
        fflush(stdout);
        printf("%d. %s en %s, de %s a %s, abierto: %s, tipo: %s\n",
               i + 1, n.nombre, n.municipio,
               n.hora_apertura, n.hora_cierre, dias, n.tipo);
    }
    printf("======================= \n");
}

/* ── Logica ── */
void gestionMenuVerNegocios(sqlite3 *db) {
    int t = 0;
    Negocio *negocios = get_negocios(db, &t);
    mostrarNegocios(negocios, t);

    printf("\nPulse Enter para volver.\n");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    if (negocios != NULL) free(negocios);
}
