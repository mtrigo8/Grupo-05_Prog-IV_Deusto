/*
 * db.c
 *
 *  Created on: 25 mar 2026
 *      Author: markel.trigo
 */

#include <stdio.h>
#include <stdlib.h>
#include "sqlite3.h"
#include <string.h>
#include  "db.h"


int registrar_log(sqlite3 *db, int id_usuario, const char *nivel, const char *mensaje) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO log (id_usuario, nivel, mensaje, fecha) VALUES (?, ?, ?, datetime('now'));";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return sqlite3_errcode(db);
    }

    sqlite3_bind_int(stmt, 1, id_usuario);
    sqlite3_bind_text(stmt, 2, nivel, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, mensaje, -1, SQLITE_STATIC);

    int rc = sqlite3_step(stmt);

    sqlite3_finalize(stmt);
    return (rc == SQLITE_DONE) ? SQLITE_OK : rc;
}

int borrar_reservas(sqlite3 *db) {
	sqlite3_stmt *stmt;

	char sql[] = "delete from reserva";

	int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) ;
	if (result != SQLITE_OK) {
		printf("Error preparing statement (DELETE)\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

	printf("SQL query prepared (DELETE)\n");

	result = sqlite3_step(stmt);
	if (result != SQLITE_DONE) {
		printf("Error deleting data\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

	result = sqlite3_finalize(stmt);
	if (result != SQLITE_OK) {
		printf("Error finalizing statement (DELETE)\n");
		printf("%s\n", sqlite3_errmsg(db));
		return result;
	}

	printf("Prepared statement finalized (DELETE)\n");

	return SQLITE_OK;
}

int insert_usuario(sqlite3 *db, char *datos[]) {
    sqlite3_stmt *stmt;
    // 1. Verificación de seguridad de entrada
    if (db == NULL || datos == NULL || datos[0] == NULL || datos[1] == NULL || datos[2] == NULL || datos[3] == NULL) {
        printf("Error: Datos de entrada nulos.\n");
        return SQLITE_ERROR;
    }

    char sql[] = "INSERT INTO usuario (id_usuario, nombre, apellido, DNI, password_hash, id_rol) VALUES (NULL, ?, ?, ?, ?, 2)";

    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error preparando el INSERT: %s\n", sqlite3_errmsg(db));
        return result;
    }


    sqlite3_bind_text(stmt, 1, datos[0], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, datos[1], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, datos[2], -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, datos[3], -1, SQLITE_TRANSIENT);

    result = sqlite3_step(stmt);

    if (result != SQLITE_DONE) {
        printf("Error al insertar usuario: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt); // SIEMPRE finalizar, incluso si falla
        return result;
    }

    sqlite3_finalize(stmt);
    printf("Usuario '%s' insertado con exito.\n", datos[0]);

    return SQLITE_OK;
}

Usuario login_usuario(sqlite3 *db, char *dni, char *contrasena) {
    sqlite3_stmt *stmt;
    Usuario u;

    memset(&u, 0, sizeof(Usuario));

    // SQL: Buscamos por nombre Y contraseña
    char sql[] = "SELECT nombre, apellido, DNI, password_hash, id_rol FROM usuario WHERE DNI = ? AND password_hash = ?";

    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error al preparar login: %s\n", sqlite3_errmsg(db));
        return u;
    }


    sqlite3_bind_text(stmt, 1, dni, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, contrasena, -1, SQLITE_TRANSIENT);

    result = sqlite3_step(stmt);

    if (result == SQLITE_ROW) {
        strcpy(u.nombre, (char *)sqlite3_column_text(stmt, 0));
        strcpy(u.apellido, (char *)sqlite3_column_text(stmt, 1));
        strcpy(u.dni, (char *)sqlite3_column_text(stmt, 2));
        strcpy(u.contrasena, (char *)sqlite3_column_text(stmt, 3));
        u.rol = sqlite3_column_int(stmt, 4);
    } else {
    	u.nombre[0] = '\0';
    }

    sqlite3_finalize(stmt);
    return u;
}

Negocio* get_negocios(sqlite3 *db, int *total_negocios) {
    sqlite3_stmt *stmt;
    int result;

    *total_negocios = 0;

    char sql_count[] = "SELECT COUNT(*) FROM servicio";
    result = sqlite3_prepare_v2(db, sql_count, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error preparando el COUNT: %s\n", sqlite3_errmsg(db));
        return NULL;
    }

    int cantidad_exacta = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        cantidad_exacta = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (cantidad_exacta == 0) {
        return NULL;
    }

    Negocio *lista = malloc(cantidad_exacta * sizeof(Negocio));
    if (lista == NULL) {
        printf("Error: No se pudo asignar memoria para %d servicios.\n", cantidad_exacta);
        return NULL;
    }

    char sql_datos[] = "SELECT nombre_servicio, municipio, hora_apertura, hora_cierre, fecha, tipo_servicio FROM servicio";
    result = sqlite3_prepare_v2(db, sql_datos, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error preparando el SELECT de datos: %s\n", sqlite3_errmsg(db));
        free(lista);
        return NULL;
    }

    int i = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && i < cantidad_exacta) {
        const char* val;

        memset(&lista[i], 0, sizeof(Negocio));

        val = (const char*)sqlite3_column_text(stmt, 0);
        if (val) strncpy(lista[i].nombre, val, sizeof(lista[i].nombre) - 1);

        val = (const char*)sqlite3_column_text(stmt, 1);
        if (val) strncpy(lista[i].municipio, val, sizeof(lista[i].municipio) - 1);

        val = (const char*)sqlite3_column_text(stmt, 2);
        if (val) strncpy(lista[i].hora_apertura, val, sizeof(lista[i].hora_apertura) - 1);

        val = (const char*)sqlite3_column_text(stmt, 3);
        if (val) strncpy(lista[i].hora_cierre, val, sizeof(lista[i].hora_cierre) - 1);

        lista[i].fecha = sqlite3_column_int(stmt, 4);

        val = (const char*)sqlite3_column_text(stmt, 5);
        if (val) strncpy(lista[i].tipo, val, sizeof(lista[i].tipo) - 1);

        i++;
    }

    sqlite3_finalize(stmt);

    *total_negocios = cantidad_exacta;

    return lista;
}

int insert_negocio(sqlite3 *db, Negocio n) {
	sqlite3_stmt *stmt;
	char sql[] = "INSERT INTO servicio (nombre_servicio, municipio, hora_apertura, hora_cierre, fecha, tipo_servicio) VALUES (?, ?, ?, ?, ?, ?)";

	int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
	    printf("Error preparando el INSERT de negocio: %s\n", sqlite3_errmsg(db));
	    return result;
	}

	sqlite3_bind_text(stmt, 1, n.nombre, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 2, n.municipio, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 3, n.hora_apertura, -1, SQLITE_TRANSIENT);
	sqlite3_bind_text(stmt, 4, n.hora_cierre, -1, SQLITE_TRANSIENT);
	sqlite3_bind_int(stmt, 5, n.fecha);
	sqlite3_bind_text(stmt, 6, n.tipo, -1, SQLITE_TRANSIENT);

	result = sqlite3_step(stmt);
	if (result != SQLITE_DONE) {
		printf("Error al insertar negocio: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);
	return result;
}

int delete_negocio(sqlite3 *db, char *nombre) {
	sqlite3_stmt *stmt;
	char sql[] = "DELETE FROM servicio WHERE nombre_servicio = ?";

	int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
	if (result != SQLITE_OK) {
	    printf("Error preparando el DELETE de negocio: %s\n", sqlite3_errmsg(db));
	    return result;
	}

	sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_TRANSIENT);

	result = sqlite3_step(stmt);
	if (result != SQLITE_DONE) {
	    printf("Error al borrar negocio: %s\n", sqlite3_errmsg(db));
	}

	sqlite3_finalize(stmt);
	return result;

}

int update_negocio(sqlite3 *db, char *nombre_actual, Negocio n_nuevo) {
    sqlite3_stmt *stmt;
    char sql[] = "UPDATE servicio SET municipio = ?, hora_apertura = ?, hora_cierre = ?, tipo_servicio = ? WHERE nombre_servicio = ?";

    int result = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (result != SQLITE_OK) {
        printf("Error preparando el UPDATE de negocio: %s\n", sqlite3_errmsg(db));
        return result;
    }

    sqlite3_bind_text(stmt, 1, n_nuevo.municipio, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, n_nuevo.hora_apertura, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, n_nuevo.hora_cierre, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, n_nuevo.tipo, -1, SQLITE_TRANSIENT);

    sqlite3_bind_text(stmt, 5, nombre_actual, -1, SQLITE_TRANSIENT);

    result = sqlite3_step(stmt);
    if (result != SQLITE_DONE) {
        printf("Error al actualizar negocio: %s\n", sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
    return result;
}
