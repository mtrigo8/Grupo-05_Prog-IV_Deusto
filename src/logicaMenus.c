#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sqlite3.h"
#include "estructuras.h"
#include "menus.h"
#include "db.h"
#include "logicaMenus.h"
#include "config.h"
#include "hash.h"

void gestionMenuBienvenida(sqlite3 *db, Config *c){
    int opcion = 0;
    int salir = 0;

    while (!salir){
        crearMenuBienvenida();
        fflush(stdout);

        {
            char _buf[16];
            fgets(_buf, sizeof(_buf), stdin);
            fflush(stdin);
            if (sscanf(_buf, "%d", &opcion) != 1) {
                opcion = 0;
            }
        }

        switch (opcion) {
            case 1:
                gestionMenuInicioSesion(db, c);
                break;
            case 2:
                gestionMenuRegistro(db);
                break;
            case 3:
                salir = 1;
                break;
            default:
                printf("Opción invalida \n\n");
        }
    }
}

void gestionMenuInicioSesion(sqlite3 *db, Config *c) {
    Usuario u_temp;
    memset(&u_temp, 0, sizeof(Usuario));

    // 1. Pedir DNI
    crearMenuInicioSesion(u_temp);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%19s", u_temp.dni);
        fflush(stdin);
    }

    // 2. Pedir contraseña
    crearMenuInicioSesion(u_temp);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%49s", u_temp.contrasena);
        fflush(stdin);
    }

    /* --- Comprobar credenciales de administrador contra el fichero de config --- */
    char hash_dni[65];
    char hash_pass[65];
    sha256_hex(u_temp.dni,       hash_dni);
    sha256_hex(u_temp.contrasena, hash_pass);

    if (strcmp(hash_dni,  c->admin_dni)      == 0 &&
        strcmp(hash_pass, c->admin_password) == 0) {
        /* LOGIN DE ADMIN CORRECTO */
        printf("\nAcceso concedido. Bienvenido, administrador.\n");
        registrar_log(db, 0, "INFO", "Inicio de sesion de administrador correcto");

        /* Rellenamos un Usuario ficticio para el menú principal */
        Usuario admin;
        memset(&admin, 0, sizeof(Usuario));
        strncpy(admin.nombre,   "Admin", sizeof(admin.nombre)   - 1);
        strncpy(admin.apellido, "",      sizeof(admin.apellido) - 1);
        strncpy(admin.dni,      u_temp.dni, sizeof(admin.dni)   - 1);

        gestionMenuPrincipal(db, admin, c);
        return;
    }

    /* --- Si no es admin, intentar login de usuario normal en BD --- */
    Usuario u_final = login_usuario(db, u_temp.dni, hash_pass);

    if (u_final.nombre[0] != '\0') {
        /* LOGIN DE USUARIO NORMAL CORRECTO */
        printf("\nBienvenido, %s.\n", u_final.nombre);
        printf("El apartado de consumidor no esta diseñado aún, intentelo más adelante.\n");
        printf("Presione Enter para volver al inicio...");
        fflush(stdout);
        getchar();
    } else {
        /* LOGIN FALLIDO */
        printf("\nError: Usuario o contraseña incorrectos.\n");
        printf("Presione Enter para volver...");
        fflush(stdout);
        getchar();
    }
}

void gestionMenuRegistro(sqlite3 *db) {
    Usuario u_temp;
    memset(&u_temp, 0, sizeof(Usuario));

    crearMenuRegistro(u_temp);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%49s", u_temp.nombre);
        fflush(stdin);
    }

    crearMenuRegistro(u_temp);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%49s", u_temp.apellido);
        fflush(stdin);
    }

    crearMenuRegistro(u_temp);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%19s", u_temp.dni);
        fflush(stdin);
    }

    crearMenuRegistro(u_temp);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%49s", u_temp.contrasena);
        fflush(stdin);
    }

    char hash[65];
    sha256_hex(u_temp.contrasena, hash);

    char *datos[4];
    datos[0] = u_temp.nombre;
    datos[1] = u_temp.apellido;
    datos[2] = u_temp.dni;
    datos[3] = hash;

    int res = insert_usuario(db, datos);

    if (res == SQLITE_OK) {
        printf("\nRegistro completado con éxito.\n");
        printf("El apartado de consumidor no esta diseñado aún, intentelo más adelante.\n");
        printf("Presione Enter para volver al inicio...");
        fflush(stdout);
        getchar();
    } else {
        printf("\nError al registrar: El DNI ya existe o hubo un problema con la base de datos.\n");
        printf("Presione Enter para volver al inicio...");
        fflush(stdout);
        getchar();
    }
}

void gestionMenuPrincipal(sqlite3 *db, Usuario u_final, Config *c){
    int opcion = 0;
    int salir = 0;

    while (!salir){
        crearMenuPrincipal();
        fflush(stdout);

        {
            char _buf[16];
            fgets(_buf, sizeof(_buf), stdin);
            fflush(stdin);
            if (sscanf(_buf, "%d", &opcion) != 1) {
                opcion = 0;
            }
        }

        switch (opcion) {
            case 1:
                gestionMenuNegocios(db, u_final);
                break;
            case 2:
                gestionMenuConfig(db, c);
                break;
            case 3:
                salir = 1;
                break;
            default:
                printf("Opción invalida \n\n");
        }
    }
}

void gestionMenuNegocios(sqlite3 *db, Usuario u_final){
    int opcion = 0;
    int salir = 0;

    while (!salir){
        crearMenuNegocios();
        fflush(stdout);

        {
            char _buf[16];
            fgets(_buf, sizeof(_buf), stdin);
            fflush(stdin);
            if (sscanf(_buf, "%d", &opcion) != 1) {
                opcion = 0;
            }
        }

        switch(opcion){
        case 1:
            gestionarMenuVerNegocio(db);
            break;
        case 2:
            gestionMenuAnyadirNegocios(db);
            break;
        case 3:
            gestionMenuEliminarNegocios(db);
            break;
        case 4:
            gestionMenuModificarNegocios(db);
            break;
        case 5:
            salir = 1;
            break;
        default:
            printf("Opción invalida \n\n");
        }
    }
}

void gestionarMenuVerNegocio(sqlite3 *db){
    int t = 0;
    Negocio *negocios = get_negocios(db, &t);
    mostrarNegocios(negocios, t);

    printf("\nPulse Enter para volver.\n");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    if (negocios != NULL) {
        free(negocios);
    }
}

void gestionMenuAnyadirNegocios(sqlite3 *db){
    Negocio n;
    memset(&n, 0, sizeof(Negocio));

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[80];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%74[^\n]", n.nombre);
        fflush(stdin);
    }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%49[^\n]", n.municipio);
        fflush(stdin);
    }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%19s", n.hora_apertura);
        fflush(stdin);
    }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%19s", n.hora_cierre);
        fflush(stdin);
    }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%49[^\n]", n.tipo);
        fflush(stdin);
    }

    char dias_temp[256];
    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    {
        char _buf[270];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%255[^\n]", dias_temp);
        fflush(stdin);
    }
    n.fecha = convertirDiasInt(dias_temp);

    int res = insert_negocio(db, n);

    if (res == SQLITE_DONE) {
        printf("\n¡Negocio '%s' añadido con éxito a la base de datos!\n", n.nombre);
    } else {
        printf("\nError: No se pudo añadir (¿Quizás ese nombre ya existe?).\n");
    }

    crearMenuAnyadirNegocios(n);
    fflush(stdout);
    getchar();
}

void gestionMenuEliminarNegocios(sqlite3 *db){
    char confirmacion;

    Negocio n_vacia;
    memset(&n_vacia, 0, sizeof(Negocio));

    crearMenuEliminarNegocios(n_vacia);
    fflush(stdout);
    {
        char _buf[80];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%74[^\n]", n_vacia.nombre);
        fflush(stdin);
    }

    crearMenuEliminarNegocios(n_vacia);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%49[^\n]", n_vacia.municipio);
        fflush(stdin);
    }

    crearMenuEliminarNegociosConfirm(n_vacia);
    {
        char _buf[8];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, " %c", &confirmacion);
        fflush(stdin);
    }

    if (confirmacion == 's' || confirmacion == 'S') {
        int res = delete_negocio(db, n_vacia.nombre);

        if (res == SQLITE_DONE) {
            if (sqlite3_changes(db) > 0) {
                printf("\n¡Se ha eliminado el negocio '%s'!\n", n_vacia.nombre);
            } else {
                printf("\nNo se encontro ningun negocio llamado '%s'.\n", n_vacia.nombre);
            }
        } else {
            printf("\nHubo un problema al intentar eliminar en la base de datos.\n");
        }
    } else {
        printf("\nOperacion cancelada. El negocio no ha sido borrado.\n");
    }

    printf("Presione Enter para volver...");
    fflush(stdout);
    getchar();
}

void gestionMenuModificarNegocios(sqlite3 *db){
    char nombre_actual[75];
    nombre_actual[0] = '\0';
    Negocio n_nuevo;
    memset(&n_nuevo, 0, sizeof(Negocio));

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[80];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%74[^\n]", nombre_actual);
        fflush(stdin);
    }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[80];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%74[^\n]", n_nuevo.nombre);
        fflush(stdin);
    }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%49[^\n]", n_nuevo.municipio);
        fflush(stdin);
    }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%19s", n_nuevo.hora_apertura);
        fflush(stdin);
    }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%19s", n_nuevo.hora_cierre);
        fflush(stdin);
    }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[64];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%49[^\n]", n_nuevo.tipo);
        fflush(stdin);
    }

    crearMenuModificarNegocios(nombre_actual, n_nuevo);
    fflush(stdout);
    {
        char _buf[270];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%255[^\n]", n_nuevo.dias);
        fflush(stdin);
    }
    n_nuevo.fecha = convertirDiasInt(n_nuevo.dias);

    int res = update_negocio(db, nombre_actual, n_nuevo);

    if (res == SQLITE_DONE && sqlite3_changes(db) > 0) {
        printf("\n¡Negocio '%s' actualizado con éxito!\n", nombre_actual);
    } else {
        printf("\nNo se pudo actualizar (¿Seguro que el negocio '%s' existe?).\n", nombre_actual);
    }

    fflush(stdout);
    getchar();
}

void gestionMenuConfig(sqlite3 *db, Config *c){
    int opcion = 0;
    int salir = 0;

    while (!salir){
        crearMenuConfig();
        fflush(stdout);

        {
            char _buf[16];
            fgets(_buf, sizeof(_buf), stdin);
            fflush(stdin);
            if (sscanf(_buf, "%d", &opcion) != 1) {
                opcion = 0;
            }
        }

        switch (opcion) {
            case 1:
                config_mostrar(c);
                printf("Presione Enter para volver...\n");
                fflush(stdout);
                getchar();
                break;
            case 2:
                gestionMenuModificarConfig(db, c);
                break;
            case 3:
                salir = 1;
                break;
            default:
                printf("Opción invalida \n\n");
        }
    }
}

void gestionMenuModificarConfig(sqlite3 *db, Config *c_actual) {
    Config c_nueva;
    memset(&c_nueva, 0, sizeof(Config));

    crearMenuModificarConfiguracion(c_nueva);
    fflush(stdout);
    {
        char _buf[270];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%255[^\n]", c_nueva.db_path);
        fflush(stdin);
    }

    char dni_admin[20];
    crearMenuModificarConfiguracion(c_nueva);
    fflush(stdout);
    {
        char _buf[32];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%19s", dni_admin);
        fflush(stdin);
    }
    sha256_hex(dni_admin, c_nueva.admin_dni);

    char pass_plana[65];
    crearMenuModificarConfiguracion(c_nueva);
    fflush(stdout);
    {
        char _buf[80];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%49s", pass_plana);
        fflush(stdin);
    }
    sha256_hex(pass_plana, c_nueva.admin_password);

    crearMenuModificarConfiguracion(c_nueva);
    fflush(stdout);
    {
        char _buf[270];
        fgets(_buf, sizeof(_buf), stdin);
        sscanf(_buf, "%255[^\n]", c_nueva.log_path);
        fflush(stdin);
    }

    crearMenuModificarConfiguracion(c_nueva);
    fflush(stdout);
    {
        char _buf[16];
        fgets(_buf, sizeof(_buf), stdin);
        fflush(stdin);
        if (sscanf(_buf, "%d", &c_nueva.max_negocios) != 1) {
            c_nueva.max_negocios = c_actual->max_negocios;
        }
    }

    crearMenuModificarConfiguracion(c_nueva);
    printf("\nGuardando cambios...\n");

    config_guardar(&c_nueva);

    printf("Pulse Enter para continuar... \n");
    fflush(stdout);
    getchar();
}
