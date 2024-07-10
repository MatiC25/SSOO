#include "kernel-init.h"

void iniciar_modulo_kernel(int socket_servidor) {
    hilo_motivo_de_desalojo();
    inicializar_lista();
    aceptar_interfaces(socket_servidor);
    inicializacion_semaforos();
    iniciar_planificacion();
}

void inicializar_lista(){
    cola_new = list_create();
    cola_ready = list_create();
    cola_prima_VRR = list_create();
    cola_block = list_create();
    cola_exit = list_create();
    inicializar_cola_resource_block();
    inicializar_vector_recursos_pedidos();
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

void liberar_comandos(COMMAND* comandos) {
    for (int i = 0; comandos[i].nombre != NULL; i++) {
        free(comandos[i].nombre);
    }
}

void finalizar_programa() {
    destruir_semaforos();
    prevent_from_memory_leaks();
    log_destroy(logger);
    log_destroy2(logger2);
    sem_destroy(&semaforo_interfaces);
    pthread_mutex_destroy(&reanudar_block);
    pthread_mutex_destroy(&reanudar_ds);
    pthread_mutex_destroy(&reanudar_largo);
    pthread_mutex_destroy(&reanudar_plani);
    free(config_kernel->IP_CPU);
    free(config_kernel->IP_KERNEL);
    free(config_kernel->IP_MEMORIA);
    free(config_kernel->RECURSOS);
    free(config_kernel->INST_RECURSOS);
    free(config_kernel->PUERTO_CPU_DS);
    free(config_kernel->PUERTO_MEMORIA);
    free(config_kernel->PUERTO_ESCUCHA);
    free(config_kernel->PUERTO_CPU_IT);
    free(config_kernel->PUERTO_KERNEL);
    free(config_kernel);

}