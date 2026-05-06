

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

//Tamaño del buffer
#define BUFF_SIZE 1024

//Separador
#define SEP "|"

/* =========================================================================
 * COMANDOS (el cliente los envia, el servidor los reconoce con strcmp)
 * ========================================================================= */

/* -- Autenticacion -------------------------------------------------------- */
#define CMD_LOGIN       "LOGIN"       /* params: dni|pass_hash               */
#define CMD_REGISTER    "REGISTER"    /* params: nombre|apellido|dni|pass_hash */
#define CMD_LOGOUT      "LOGOUT"      /* params: id_usuario                  */

/* -- Servicios ------------------------------------------------------------ */
#define CMD_GET_SERVICIOS    "GET_SERVICIOS"    /* params: tipo_servicio      */
#define CMD_GET_SERVICIO     "GET_SERVICIO"     /* params: id_servicio        */
#define CMD_FILTER_SERVICIOS "FILTER_SERVICIOS" /* params: tipo|fecha         */

/* -- Reservas ------------------------------------------------------------- */
#define CMD_CREATE_RESERVA   "CREATE_RESERVA"   /* params: id_usuario|id_servicio       */
#define CMD_GET_RESERVA      "GET_RESERVA"      /* params: id_usuario                   */
#define CMD_CANCEL_RESERVA   "CANCEL_RESERVA"   /* params: id_reserva|id_usuario        */
#define CMD_UPDATE_RESERVA   "UPDATE_RESERVA"   /* params: id_reserva|id_usuario|id_nuevo */

/* -- Conexion ------------------------------------------------------------- */
#define CMD_PING        "PING"        /* sin parametros                      */
#define CMD_DISCONNECT  "DISCONNECT"  /* params: id_usuario                  */

/* =========================================================================
 * RESPUESTAS DEL SERVIDOR
 * ========================================================================= */

/* -- Respuestas de exito -------------------------------------------------- */
#define RES_OK              "OK"
#define RES_PONG            "PONG"
#define RES_SESION_CERRADA  "OK|SESION_CERRADA"
#define RES_CANCELADA       "OK|CANCELADA"
#define RES_DESCONECTADO    "OK|DESCONECTADO"

/* -- Respuestas de error -------------------------------------------------- */
#define RES_ERR_CREDENCIALES "ERR|CREDENCIALES_INVALIDAS"
#define RES_ERR_DNI_DUP      "ERR|DNI_DUPLICADO"
#define RES_ERR_NO_ENCONTRADO "ERR|NO_ENCONTRADO"
#define RES_ERR_SIN_CUPOS    "ERR|SIN_CUPOS"
#define RES_ERR_YA_RESERVADO "ERR|YA_RESERVADO"
#define RES_ERR_NO_CANCELABLE "ERR|NO_CANCELABLE"
#define RES_ERR_NO_AUTORIZADO "ERR|NO_AUTORIZADO"
#define RES_ERR_GENERICO     "ERR|ERROR"


#define RES_LIST_START  "LIST_START"
#define RES_LIST_END    "LIST_END"

#endif /* PROTOCOL_H_ */
