/*
 * menu_ver_negocios.c
 *
 * Vista y logica del menu de visualizacion de negocios.
 */

#include "menu_ver_negocios.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "negocio.h"
#include "sqlite3.h"

/* ── Vista ── */
void mostrarNegocios(Negocio *negocios, int cantidad_total) {
    printf("======================= \n");
    printf("Mostrando todos los negocios en la base de datos\n");
    printf("======================= \n");

    for (int i = 0; i < cantidad_total; i++) {
        Negocio n = negocios[i];          /* copia de punteros, solo para lectura */
        char dias[100];
        memset(dias, 0, sizeof(dias));
        convertirIntDias(n.fecha, dias);
        fflush(stdout);
        printf("%d. %s en %s, de %s a %s, abierto: %s, tipo: %s\n",
               i + 1,
               n.nombre        ? n.nombre        : "(sin nombre)",
               n.municipio     ? n.municipio     : "(sin municipio)",
               n.hora_apertura ? n.hora_apertura : "--:--",
               n.hora_cierre   ? n.hora_cierre   : "--:--",
               dias,
               n.tipo          ? n.tipo          : "(sin tipo)");
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

    /* Liberar primero los campos dinamicos de cada negocio,
     * luego el propio array asignado por get_negocios(). */
    if (negocios != NULL) {
        for (int i = 0; i < t; i++)
            negocio_free(&negocios[i]);
        free(negocios);
    }
}
