#include "kernel-peticiones-dispatch.h"

void hilo_motivo_de_desalojo() {
    pthread_t hilo_desalojo;
    pthread_create(&hilo_desalojo, NULL, escuchar_peticiones_dispatch, NULL);
    pthread_detach(&hilo_desalojo);
}


void* escuchar_peticiones_dispatch() {
    while (1) {

        t_pcb *pcb = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
        if (!pcb) {
            log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        }

        tipo_desalojo motivo_desalojo = recibir_operacion(config_kernel->SOCKET_DISPATCH);
        if (motivo_desalojo < 0) {
            log_error(logger, "Dispatch acaba de recibir un motivo de desalojo invalido!");
        }
        
        const char* tipo_de_exit = transformar_motivos_a_exit(&motivo_desalojo);

        switch(motivo_desalojo) {  

            case FIN_QUANTUM:
                peticion_fin_quantum(pcb);
                break;
                 
            case INSTRUCCION_IO:
                peticion_IO(pcb); // TODO
                break;
            
            case WAIT:
                peticion_wait();
                break;

            case SIGNAL:
                peticion_signal();
                break;
            
            case FIN_DE_PROCESO:
                peticion_end(pcb, tipo_de_exit);
                break;
            
            default:
                log_error(logger, "El motivo de desalojo no existe!");
                break;
        }
        sem_post(&desalojo_proceso);
    }
    return NULL;
}


const char* transformar_motivos_a_exit(tipo_desalojo *motivo_inicial) {
    switch(*motivo_inicial) {
        case FIN_DE_PROCESO: 
            *motivo_inicial = FIN_DE_PROCESO;
            return "SUCCESS";
        
        case OUT_OF_MEMORY:
            *motivo_inicial = FIN_DE_PROCESO;
            return "OUT_OF_MEMORY";
        
        case IO_INVALIDA:
            *motivo_inicial = FIN_DE_PROCESO;
            return "IO_INVALIDA";
        
        case RECURSO_INVALIDO:
            *motivo_inicial = FIN_DE_PROCESO;
            return "RECURSO_INVALIDO";

        case INTERRUPTED_BY_USER:
            *motivo_inicial = FIN_DE_PROCESO;
            return "INTERRUPTED_BY_USER";
        
        default:
            return "UNKNOWN_ERROR";
    }
}


void peticion_fin_quantum(t_pcb* contexto) {

    log_info(logger, "PID: %i - Desalojado por fin de Quantum", contexto->pid);

    // Reinsertar el proceso en la cola de READY
    pthread_mutex_lock(&mutex_estado_ready);
    list_add(cola_ready, contexto);
    contexto->estado = READY;
    log_info(logger, "PID: %i - Estado Anterior: EXEC - Estado Actual: READY", contexto->pid);
    pthread_mutex_unlock(&mutex_estado_ready);

    sem_post(&hay_en_estado_ready);

}


void peticion_end(t_pcb* contexto, const char* tipo_de_exit) { 
    log_info(logger, "Finaliza el proceso %i - Motivo: %s", contexto->pid, tipo_de_exit);

    informar_a_memoria_liberacion_proceso(contexto -> pid);

    free(contexto -> registros);
    free(contexto);

    sem_post(&limite_grado_multiprogramacion);
}


void peticion_wait() {

}


void peticion_signal() {

}


void peticion_IO(t_pcb *pcb) {
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

