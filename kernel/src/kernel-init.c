#include "kernel-init.h"

void iniciar_modulo_kernel(int socket_servidor) {
    hilo_motivo_de_desalojo();
    inicializar_lista();
    aceptar_interfaces(socket_servidor);
    inicializacion_semaforos();
    iniciar_planificacion();
    iniciar_consola();
    //manejar_peticion_con_memoria();
}

void inicializar_lista(){
    cola_new = list_create();
    cola_ready = list_create();
    cola_prima_VRR = list_create();
    cola_block = list_create();
    cola_exec = list_create();
}

void aceptar_interfaces(int socket_servidor) {
    pthread_t aceptar_interfaces_thread;

    pthread_create(&aceptar_interfaces_thread, NULL, (void *) handle_new_interface, (void *) socket_servidor);
    pthread_detach(aceptar_interfaces_thread);   
}

void iniciar_planificacion() {

    pthread_t planificacion_corto_plazo_thread;
    pthread_t planificacion_largo_plazo_thread;

    //Planificación a corto plazo y largo plazo:
    pthread_create(&planificacion_corto_plazo_thread, NULL, elegir_algoritmo_corto_plazo, NULL);
    pthread_create(&planificacion_largo_plazo_thread, NULL, agregar_a_cola_ready, NULL); 
    
    //Esperamos a que terminen los hilos de planificación:
    pthread_detach(planificacion_corto_plazo_thread);
    pthread_detach(planificacion_largo_plazo_thread);
}






