#include "kernel-peticiones-dispatch.h"

void hilo_motivo_de_desalojo() {
    pthread_t hilo_desalojo;
    pthread_create(&hilo_desalojo, NULL, escuchar_peticiones_dispatch, NULL);
    pthread_detach(&hilo_desalojo);
}


void* escuchar_peticiones_dispatch() {
    while (1) {

        tipo_desalojo motivo_desalojo = recibir_operacion(config_kernel->SOCKET_DISPATCH);
        if (motivo_desalojo < 0) {
            log_error(logger, "Dispatch acaba de recibir un motivo de desalojo invalido!");
        }

        const char* tipo_de_exit = transformar_motivos_a_exit(&motivo_desalojo);

        switch(motivo_desalojo) {  

            case FIN_QUANTUM:
                peticion_fin_quantum();
                break;
                 
            case INSTRUCCION_IO:
                peticion_IO(); // TODO
                break;
            
            case WAIT:
                peticion_wait(); // TODO
                break;

            case SIGNAL:
                peticion_signal(); // TODO
                break;
            
            case FIN_DE_PROCESO:
                peticion_exit(tipo_de_exit);
                break;
            
            default:
                log_error(logger, "El motivo de desalojo no existe!");
                break;
        }

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


void peticion_fin_quantum() {

    pthread_mutex_lock(&mutex_proceso_exec);
    proceso_en_exec = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
    pthread_mutex_unlock(&mutex_proceso_exec);

    if (!proceso_en_exec) {  
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
    }

    sem_post(&desalojo_proceso);

    log_info(logger, "PID: %i - Desalojado por fin de Quantum", proceso_en_exec->pid);

    // Reinsertar el proceso en la cola de READY
    pthread_mutex_lock(&mutex_estado_ready);
    list_add(cola_ready, proceso_en_exec);

    pthread_mutex_lock(&mutex_proceso_exec);
    proceso_en_exec->quantum = 0;
    proceso_en_exec->estado = READY;
    pthread_mutex_unlock(&mutex_proceso_exec);

    log_info(logger, "PID: %i - Estado Anterior: EXEC - Estado Actual: READY", proceso_en_exec->pid);
    pthread_mutex_unlock(&mutex_estado_ready);

    sem_post(&hay_en_estado_ready);

}


void peticion_exit(const char* tipo_de_exit) { 
    t_pcb *pcb = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
    if (!pcb) {  
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
    }

    log_info(logger, "Finaliza el proceso %i - Motivo: %s", pcb->pid, tipo_de_exit);

    informar_a_memoria_liberacion_proceso(pcb -> pid); // PREGUNTAR

    free(pcb -> registros);
    free(pcb);

    sem_post(&limite_grado_multiprogramacion);
}


void peticion_wait() {
    char* recurso;

    t_pcb *pcb = recibir_contexto_y_recurso(&recurso); 
    if (!pcb) {  
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        return;
    }
    
    if (!recurso_existe(recurso)) {
        log_error(logger, "El recurso solicitado no existe!");
        mover_a_exit(pcb); // TODO
        free(recurso);
        return;
    }

    // Obtener el índice del recurso en la lista
    int indice_recurso = obtener_indice_recurso(recurso);

    // Verificar la cantidad de instancias
    int* instancias = list_get(config_kernel->INST_RECURSOS, indice_recurso);
    if (*instancias > 0) {
        // Hay instancias disponibles => restar una instancia
        (*instancias)--;
    } else {
        // No hay instancias disponibles => bloquear el proceso
        mover_a_bloqueado_por_wait(pcb, recurso); // TODO
        free(recurso);
        return;
    }

    // Resto del manejo del proceso que hace WAIT...
    free(recurso);
}

void peticion_signal() {
    char* recurso;

    t_pcb *pcb = recibir_contexto_y_recurso(&recurso); 
    if (!pcb) {  
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        return;
    }
    
    if (!recurso_existe(recurso)) {
        log_error(logger, "El recurso solicitado no existe!");
        mover_a_exit(pcb); // TODO
        free(recurso);
        return;
    }

    // Obtener el índice del recurso en la lista
    int indice_recurso = obtener_indice_recurso(recurso);

    // Incrementar una instancia del recurso
    int* instancias = list_get(config_kernel->INST_RECURSOS, indice_recurso);
    (*instancias)++;

    //FALTA EL TEMA DE GESTION DE LISTAS BLOQUEADAS

    free(recurso);
}


int obtener_indice_recurso(char* recurso) {
    for (int i = 0; i < list_size(config_kernel->RECURSOS); i++) {
        char* recurso_actual = list_get(config_kernel->RECURSOS, i);
        if (strcmp(recurso_actual, recurso) == 0) {
            return i;  // Se encontró el recurso, devolver el índice
        }
    }
    return -1;  // Si no se encuentra el recurso, devolver -1
}


bool recurso_existe(char* recurso) {
    return obtener_indice_recurso(recurso) != -1;
}


t_pcb* recibir_contexto_y_recurso(char** recurso) {
    t_pcb* proceso = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
    if (!proceso) {
        log_error(logger, "Error al recibir el contexto de ejecución");
        return NULL;
    }

    *recurso = rcv_nombre_recurso(config_kernel->SOCKET_DISPATCH);
    if (!*recurso) {
        log_error(logger, "Error al recibir el nombre del recurso");
        free(proceso->registros);
        free(proceso);
        return NULL;
    }

    return proceso;
}


void peticion_IO(t_pcb *pcb_bloqueado) {
    char *interface_name;
    
    // recibimos el nombre de la interfaz:
    rcv_interface(&interface_name, config_kernel->SOCKET_DISPATCH);

    interface_io *interface = get_interface_from_dict(interface_name);
    tipo_operacion operacion = recibir_operacion(); 

    // Verificamos que la interfaz exista:
    if (consulta_existencia_interfaz(interface) && consulta_interfaz_para_aceptacion_de_operacion(interface)) {
        move_pcb_to_exit(pcb_bloqueado); // Si no existe la interfaz o no acepta la operacion, mover a exit!

        return NULL; // Salimos de la funcion
    }
    
    t_list *args = rcv_args(); // Recibimos los argumentos de la operacion

    // Si la interfaz existe y acepta la operacion, procedemos a ejecutarla:
    queue_push(interface->process_blocked, pcb_bloqueado);
    queue_push(interface->args_process, args); // Agregamos los argumentos a la cola de argumentos
    sem_post(&interface->size_blocked); // Aumentamos el tamaño de la cola de bloqueados
}


void rcv_interface(char **interface_name, int socket) {
    int tamanio;
    int desplazamiento = 0;
    int size = 0;
    void *buffer = recibir_buffer(&size, socket);
    
    recibir_nombre_interfaz(interface_name, buffer, &desplazamiento, &tamanio);
}