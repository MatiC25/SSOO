#include "instrucciones.h"

void send_instruccion(int socket_cliente, char *instruccion, t_list *parametros) {
    t_tipo_instruccion tipo_instruccion = obtener_tipo_instruccion(instruccion);
    t_paquete *paquete = crear_paquete(tipo_instruccion);

    agregar_a_paquete_lista_string(paquete, parametros);
    eliminar_paquete(paquete);
}

t_instruccion* recv_instruccion(int socket_cliente) {
    t_instruccion* instruccion = inicializar_instruccion();
    instruccion->tipo = recibir_operacion(socket_cliente);
    instruccion->parametros = recv_list(socket_cliente);

    return instruccion;
}

t_instruccion *inicializar_instruccion() {
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    instruccion->parametros = list_create();
    return instruccion;
}

void solicitar_instruccion(int socket_server, int PID, int program_counter) {
    t_paquete *paquete = crear_paquete(SOLICITAR_INSTRUCCION);
    agregar_a_paquete(paquete, PID, sizeof(int));
    agregar_a_paquete(paquete, program_counter, sizeof(int));
    enviar_paquete(socket_server, paquete);
    eliminar_paquete(paquete);
}

t_tipo_instruccion obtener_tipo_instruccion(char *instruccion) {
    if(strcmp(instruccion, "SET") == 0) {
        return SET;
    } else if(strcmp(instruccion, "GET") == 0) {
        return GET;
    } else if(strcmp(instruccion, "SUM") == 0) {
        return SUM;
    } else if(strcmp(instruccion, "SUB") == 0) {
        return SUB;
    } else if(strcmp(instruccion, "JNZ") == 0) {
        return JNZ;
    } else if(strcmp(instruccion, "IO_GEN_SLEEP") == 0) {
        return IO_GEN_SLEEP;
    }
}