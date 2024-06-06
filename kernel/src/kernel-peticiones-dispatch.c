#include "kernel-peticiones-dispatch.h"

void hilo_motivo_de_desalojo() {
    pthread_t hilo_desalojo;
    pthread_create(&hilo_desalojo, NULL, escuchar_peticiones_dispatch, NULL);
    pthread_detach(&hilo_desalojo);
}

void* escuchar_peticiones_dispatch() {
    while (1) {
        // Recibimos el PCB desde el socket de dispatch
        t_pcb *pcb = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
        if (!pcb) {
            log_error(logger, "Dispatch acaba de recibir algo inexistente!");
            continue; // Continuar a la próxima iteración
        }

        // Recibimos el motivo del desalojo
        tipo_desalojo motivo_desalojo = recibir_operacion(config_kernel->SOCKET_DISPATCH);
        if (motivo_desalojo < 0 || motivo_desalojo >= (sizeof(diccionario_peticiones) / sizeof(COMMAND))) {
            log_error(logger, "Dispatch acaba de recibir un motivo de desalojo inexistente!");
            continue; // Continuar a la próxima iteración
        }

        // Llamamos a la función correspondiente
        diccionario_peticiones[motivo_desalojo].funcion(pcb);
    }
    return NULL;
}


COMMAND diccionario_peticiones[] = {
    {IO, peticion_IO},
    {FIN_QUANTUM, peticion_fin_quantum},
    {WAIT, peticion_wait},
    {SIGNAL, peticion_signal},
    {FIN_EJECUCION, peticion_end}
};


void* peticion_fin_quantum(t_pcb* contexto) {
    sem_post(&pcb_ya_recibido);
    log_info(logger, "PID: %i - Desalojado por fin de Quantum", contexto->pid);

    // Reinsertar el proceso en la cola de READY
    pthread_mutex_lock(&mutex_estado_ready);
    list_add(cola_ready, contexto);
    log_info(logger, "PID: %i - Estado Anterior: EXEC - Estado Actual: READY", contexto->pid);
    pthread_mutex_unlock(&mutex_estado_ready);

    sem_post(&nuevo_pcb_a_ejecutar);  // Indicar que hay un proceso en READY nuevamente

    return NULL; // Asegurarse de que la función devuelva algo
}


void* peticion_end() { //NO SE COMO MANEJAR LSO DISTINTOS CASOS DE EXIT
    

}


void* peticion_wait() {

}


void* peticion_signal() {

}


void* peticion_signal() {

}


void* peticion_IO(t_pcb *pcb) {
    // char *interface_name = recibir_interface_name();
    // interface_io *interface = get_interface_from_dict(interface_name);
    // tipo_operacion operacion = recibir_operacion(); 

    // // Verificamos que la interfaz exista:
    // if (consulta_existencia_interfaz(interface)) {
    //     move_pcb_to_exit();

    //     return NULL; // Salimos de la funcion
    // }

    // // Verificamos que la operacion sea valida:
    // if(consulta_interfaz_para_aceptacion_de_operacion(interface, operacion)) {
        

    // } else
    //     move_pcb_to_exit();
}
