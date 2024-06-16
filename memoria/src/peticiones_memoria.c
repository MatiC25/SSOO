#include "peticiones_memoria.h"

//Estructuras

void* escuchar_peticiones(void* args){
    int socket_cliente = *(int*) args;
    while (1)
    {
        int cod_op = recibir_operacion(socket_cliente);
        log_warning(logger,"cod op %i",cod_op);
        switch (cod_op){
        case HANDSHAKE:
            log_warning(logger,"HANDSHAKE");
            recibir_handshake(socket_cliente);
            break;
        case MENSAJE:
            log_warning(logger,"MENSAJE");
            recibir_mensaje(socket_cliente);
            log_warning(logger,"enviado mensaje");
            enviar_mensaje("MEMORIA -> CPU", socket_cliente);
            break;
        case HANDSHAKE_PAGINA:
            log_warning(logger,"HANDSHAKE_PAGINA");
            recibir_handshake(socket_cliente);
            handshake_desde_memoria(socket_cliente);
            break;
        case INICIAR_PROCESO:
            // retardo_pedido(config_memoria -> retardo_respuesta);
            // crear_proceso(socket_cliente);
            leer_archivoPseudo(socket_cliente);
            break;
        case FINALIZAR_PROCESO:
            //retardo_pedido(config_memoria -> retardo_respuesta);
            //terminar_proceso(socket_cliente);
            break;
        case INSTRUCCION: 
            //El retardo ya esta incluido en la funcion
            enviar_instruccion_a_cpu(socket_cliente, config_memoria->retardo_respuesta);
            break;
        case ACCEDER_TABLA_PAGINAS: 
            //retardo_pedido(config_memoria -> retardo_respuesta);
            //obtener_marco(socket_cliente);
            break;
        case MODIFICAR_TAMAÑO_MEMORIA:
            //sretardo_pedido(config_memoria -> retardo_respuesta);
            //resize_proceso(socket_cliente);
            break;
        case ACCESO_A_LECTURA:
            //retardo_pedido(config_memoria -> retardo_respuesta);
            //acceso_lectura(socket_cliente);
            break;
        case ACCESO_A_ESCRITURA:
            //retardo_pedido(config_memoria -> retardo_respuesta);
            //sacceso_escritura(socket_cliente);
            break;
        case -1:
            log_info(logger,"Se desconecto el cliente");
            exit(-1); //a facu le gusta exit(-1) pero nico nos amenaza para poner return
        default:
            log_error(logger, "Operacion desconocida");
        }
    }
}

void handshake_desde_memoria(int socket_cliente) {
    t_paquete* paquete = crear_paquete(HANDSHAKE_PAGINA);
    int tam_pagina = config_memoria ->tam_pagina;

    // send(socket_cliente, &tam_pagina, sizeof(int), 0);
    
    agregar_a_paquete(paquete, &tam_pagina, sizeof(int));
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}