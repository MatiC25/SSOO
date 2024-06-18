#include "memoria_peticiones.h"

sem_t enviar_instruc;
sem_t sem_lectura_archivo;

void inicializar_semaforo() {
    sem_init(&enviar_instruc, 0, 0);
    sem_init(&sem_lectura_archivo, 0, 1);
}
/* Initialize semaphore object SEM to VALUE.  If PSHARED then share it
   with other processes.  */
//extern int sem_init (sem_t *__sem, int __pshared, unsigned int __value)

void* escuchar_peticiones(void* args){
    int socket_cliente = *(int*) args;
    inicializar_semaforo(); 

    while (1){   
        
        int cod_op = recibir_operacion(socket_cliente);
        //log_warning(logger,"cod op %i",cod_op);
        switch (cod_op){
        case HANDSHAKE:
            recibir_handshake(socket_cliente);
            break;
        case MENSAJE:
            recibir_mensaje(socket_cliente);
            //enviar_mensaje("MEMORIA -> CPU", socket_cliente);
            break;
        case HANDSHAKE_PAGINA:
            //log_warning(logger,"HANDSHAKE_PAGINA");
            recibir_handshake(socket_cliente);
            handshake_desde_memoria(socket_cliente);
            break;
        case INICIAR_PROCESO:
            // /sem_wait(&sem_lectura_archivo);
            log_info(logger, "Iniciando proceso");
            retardo_pedido(config_memoria -> retardo_respuesta);
            // crear_proceso(socket_cliente);
            leer_archivoPseudo(socket_cliente);
            enviar_respuesta_a_kernel(socket_cliente);
            //sem_post(&enviar_instruc);
            break;
        case FINALIZAR_PROCESO:
            retardo_pedido(config_memoria -> retardo_respuesta);
            //terminar_proceso(socket_cliente);
            break;
        case INSTRUCCION: 
            // retardo_pedido(config_memoria -> retardo_respuesta);
            // sem_wait(&enviar_instruc);
            log_info(logger, "Enviando instruccion");
            enviar_instruccion_a_cpu(socket_cliente);
            // sem_post(&sem_lectura_archivo);
            break;
        case ACCEDER_TABLA_PAGINAS: 
            retardo_pedido(config_memoria -> retardo_respuesta);
            //obtener_marco(socket_cliente);
            break;
        case MODIFICAR_TAMAÑO_MEMORIA:
            retardo_pedido(config_memoria -> retardo_respuesta);
            //resize_proceso(socket_cliente);
            break;
        case ACCESO_A_LECTURA:
            retardo_pedido(config_memoria -> retardo_respuesta);
            //acceso_lectura(socket_cliente);
            break;
        case ACCESO_A_ESCRITURA:
            retardo_pedido(config_memoria -> retardo_respuesta);
            //acceso_escritura(socket_cliente);
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

void enviar_respuesta_a_kernel(int socket_cliente) {
    int respuesta = 1;

    send(socket_cliente, &respuesta, sizeof(int), 0);
}