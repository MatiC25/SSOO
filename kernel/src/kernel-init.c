#include "kernel-init.h"

t_list* cola_new = NULL;
t_list* cola_ready = NULL;
t_list* cola_block = NULL;
t_list* cola_prima_VRR = NULL;


void iniciar_modulo_kernel(int socket_servidor) {
    inicializar_lista();
    aceptar_interfaces(socket_servidor);
    
    //manejar_peticion_con_memoria();
    // manejar_peticion_con_cpu();
    // iniciar_planificacion();
}

void inicializar_lista(){
    cola_new = list_create();
    cola_ready = list_create();
    cola_block = list_create();
    cola_prima_VRR = list_create();
}


void aceptar_interfaces(int socket_servidor) {
    pthread_t aceptar_interfaces_thread;

    pthread_create(&aceptar_interfaces_thread, NULL, handle_new_interface,(void *)(intptr_t)socket_servidor);
    pthread_join(aceptar_interfaces_thread, NULL);    
}

void iniciar_planificacion() {

    pthread_t planificacion_corto_plazo_thread;
    pthread_t planificacion_largo_plazo_thread;

    //Planificación a corto plazo y largo plazo:
    pthread_create(&planificacion_corto_plazo_thread, NULL, elegir_algoritmo_corto_plazo, NULL);
    // pthread_create(&planificacion_largo_plazo_thread, NULL, , NULL); 
    
    //Esperamos a que terminen los hilos de planificación:
    pthread_join(planificacion_corto_plazo_thread, NULL);
    pthread_join(planificacion_largo_plazo_thread, NULL);
}

void manejar_peticion_con_memoria() {
    pthread_t memoria_thread;

    pthread_create(&memoria_thread, NULL,creacion_proceso, NULL);
    pthread_join(memoria_thread, NULL);
}

void manejar_peticion_con_cpu() {
    pthread_t cpu_thread;

    pthread_create(&cpu_thread, NULL,enviar_proceso_a_cpu, NULL);
    pthread_join(cpu_thread, NULL);
}





