#include "kernel-peticiones-dispatch.h"

void hilo_motivo_de_desalojo() {
    pthread_t hilo_desalojo;
    pthread_create(&hilo_desalojo, NULL, escuchar_peticiones_dispatch, NULL);
    pthread_detach(hilo_desalojo);
}


void* escuchar_peticiones_dispatch() {

    while (1) {
        t_tipo_instruccion motivo_desalojo = recibir_operacion(config_kernel->SOCKET_DISPATCH);

        if (motivo_desalojo < 0) {
            log_error(logger, "Dispatch acaba de recibir un motivo de desalojo inválido!");
            continue;
        }

        const char* tipo_de_exit = transformar_motivos_a_exit(&motivo_desalojo);

        switch (motivo_desalojo) {
            case FIN_QUANTUM:
                peticion_fin_quantum();
                break;
            case INSTRUCCION_IO:
                peticion_IO();
                break;
            case WAIT:
                peticion_wait();
                break;
            case SIGNAL:
                peticion_signal();
                break;
            case EXIT:
                peticion_exit(tipo_de_exit);
                break;
            default:
                log_error(logger, "El motivo de desalojo no existe!");
                break;
        }
    }
    return NULL;
}


const char* transformar_motivos_a_exit(t_tipo_instruccion* motivo_inicial) {

    switch (*motivo_inicial) {

        case EXIT:
            return "SUCCESS";

        case OUT_OF_MEMORY:
            *motivo_inicial = EXIT;
            return "OUT_OF_MEMORY";

        case INTERRUPTED_BY_USER:
            *motivo_inicial = EXIT;
            return "INTERRUPTED_BY_USER";

        default:
            return "UNKNOWN_ERROR";
    }
}


void peticion_fin_quantum() {

    pthread_mutex_lock(&mutex_proceso_exec);
    t_pcb* proceso_actual = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
    pthread_mutex_unlock(&mutex_proceso_exec);

    if (!proceso_actual) {
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        return;
    }

    sem_post(&desalojo_proceso);

    log_info(logger, "PID: %i - Desalojado por fin de Quantum", proceso_actual->pid);

    // Reinsertar el proceso en la cola de READY
    pthread_mutex_lock(&mutex_estado_ready);

    pthread_mutex_lock(&mutex_proceso_exec);
    proceso_actual->quantum = 0;
    proceso_actual->estado = READY;
    pthread_mutex_unlock(&mutex_proceso_exec);

    list_add(cola_ready, proceso_actual);
    log_info(logger, "PID: %i - Estado Anterior: EXEC - Estado Actual: READY", proceso_actual->pid);

    pthread_mutex_unlock(&mutex_estado_ready);

    sem_post(&hay_en_estado_ready);
}


void peticion_exit(const char* tipo_de_exit) {

    t_pcb* pcb = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);

    if (!pcb) {
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        return;
    }

    log_info(logger, "Finaliza el proceso %i - Motivo: %s", pcb->pid, tipo_de_exit);

    // Verificar si el proceso tiene algún recurso asociado
    liberar_recurso_por_exit(pcb);

    informar_a_memoria_liberacion_proceso(pcb->pid);

    free(pcb->registros);
    free(pcb);

    sem_post(&sem_multiprogramacion);
}


void peticion_wait() {

    char* recurso;
    t_pcb* pcb = recibir_contexto_y_recurso(&recurso);
    if (!pcb) {
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        return;
    }

    if (!recurso_existe(recurso)) {
        log_error(logger, "El recurso solicitado no existe!");
        finalizar_por_invalidacion(pcb, "INVALID_RESOURCE");
        free(recurso);
        return;
    }

    int indice_recurso = obtener_indice_recurso(recurso);

    if (config_kernel->INST_RECURSOS[indice_recurso] > 0) {
        config_kernel->INST_RECURSOS[indice_recurso]--;

        // Guardar el pid y el recurso en el vector
        for (int i = 0; i < tam_vector_recursos_pedidos; i++) {
            if (vector_recursos_pedidos[i].PID == -1) {
                vector_recursos_pedidos[i].PID = pcb->pid;
                vector_recursos_pedidos[i].recurso = strdup(recurso);
                break;
            }
        }

        enviar_proceso_a_cpu(pcb);

    } else {
        mover_a_bloqueado_por_wait(pcb, recurso);
        free(recurso);
        return;
    }

    free(recurso);
}


void peticion_signal() {

    char* recurso;
    t_pcb* pcb = recibir_contexto_y_recurso(&recurso);
    if (!pcb) {
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        return;
    }

    if (!recurso_existe(recurso)) {
        log_error(logger, "El recurso solicitado no existe!");
        finalizar_por_invalidacion(pcb, "INVALID_RESOURCE");
        free(recurso);
        return;
    }

    int indice_recurso = obtener_indice_recurso(recurso);
    config_kernel->INST_RECURSOS[indice_recurso]++;

    // Liberar el pid y el recurso en el vector
    for (int i = 0; i < tam_vector_recursos_pedidos; i++) {
        if (vector_recursos_pedidos[i].PID == pcb->pid && strcmp(vector_recursos_pedidos[i].recurso, recurso) == 0) {
            vector_recursos_pedidos[i].PID = -1;
            free(vector_recursos_pedidos[i].recurso);
            vector_recursos_pedidos[i].recurso = NULL;
            break;
        }
    }

    if (!queue_is_empty(colas_resource_block[indice_recurso])) {
        pthread_mutex_lock(&mutex_estado_block);
        t_pcb* pcb_signal = queue_pop(colas_resource_block[indice_recurso]);
        pthread_mutex_unlock(&mutex_estado_block);

        pthread_mutex_lock(&mutex_estado_ready);
        pcb_signal->estado = READY;
        list_add(cola_ready, pcb_signal);
        pthread_mutex_unlock(&mutex_estado_ready);

        log_info(logger, "PID: %i - Estado Anterior: BLOCK - Estado Actual: READY", pcb_signal->pid);
        sem_post(&hay_en_estado_ready);
    }

    enviar_proceso_a_cpu(pcb);
    free(recurso);
}


void mover_a_bloqueado_por_wait(t_pcb* pcb, char* recurso) {
    log_info(logger, "PID: %i - Bloqueado por: %s", pcb->pid, recurso);
    int indice_recurso = obtener_indice_recurso(recurso);

    pthread_mutex_lock(&mutex_estado_block);
    pcb->estado = BLOCK;
    queue_push(colas_resource_block[indice_recurso], pcb);
    pthread_mutex_unlock(&mutex_estado_block);

    log_info(logger, "PID: %i - Estado Anterior: EXEC - Estado Actual: BLOCK", pcb->pid);
}


void inicializar_colas_bloqueados() {
    for (int i = 0; i < MAX_RECURSOS; i++) {
        colas_resource_block[i] = queue_create();
    }
}


int obtener_indice_recurso(char* recurso) {
    for (int i = 0; i < MAX_RECURSOS; i++) {
        if (strcmp(config_kernel->RECURSOS[i], recurso) == 0) {
            return i;
        }
    }
    return -1;
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

    // Recibimos el nombre del recurso:
    rcv_nombre_recurso(recurso, config_kernel->SOCKET_DISPATCH);

    if (!*recurso) {
        log_error(logger, "Error al recibir el nombre del recurso");
        free(proceso->registros);
        free(proceso);
        return NULL;
    }

    return proceso;
}


void peticion_IO() {
    t_pcb* pcb_bloqueado = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
    
    if (!pcb_bloqueado) {
        log_error(logger, "Error al recibir el contexto de ejecución para IO");
        return;
    }

    char* interface_name;
    rcv_nombre_interfaz_dispatch(&interface_name, config_kernel->SOCKET_DISPATCH);
    
    interface_io* interface = get_interface_from_dict(interface_name);
    if (!interface) {
        peticion_exit("INVALID_INTERFACE");
        free(interface_name);
        return;
    }

    tipo_operacion operacion = recibir_operacion(config_kernel->SOCKET_DISPATCH);
    if (!acepta_operacion_interfaz(interface, operacion)) {
        peticion_exit("INVALID_OPERATION");
        free(interface_name);
        return;
    }

    t_list* args = rcv_argumentos_para_io(interface->tipo, config_kernel->SOCKET_DISPATCH);

    pthread_mutex_lock(&mutex_estado_block);
    queue_push(interface->process_blocked, pcb_bloqueado);
    queue_push(interface->args_process, args);
    sem_post(&interface->size_blocked);
    pthread_mutex_unlock(&mutex_estado_block);

    free(interface_name);
}


void rcv_nombre_recurso(char** recurso, int socket) {
    int tamanio;

    // Recibir el tamaño del nombre del recurso
    if (recv(socket, &tamanio, sizeof(int), MSG_WAITALL) <= 0) {
        *recurso = NULL;
        log_error(logger, "No se recibió el tamaño del nombre del recurso: %s", strerror(errno));
        return;
    }

    // Reservar memoria para el nombre del recurso
    *recurso = malloc(tamanio);
    if (*recurso == NULL) {
        log_error(logger, "Error al reservar memoria para el recurso: %s", strerror(errno));
        return;
    }

    // Recibir el nombre del recurso
    if (recv(socket, *recurso, tamanio, MSG_WAITALL) <= 0) {
        log_error(logger, "No se recibió el nombre del recurso: %s", strerror(errno));
        free(*recurso);
        *recurso = NULL;
    } else {
        log_info(logger, "Se recibió el nombre del recurso: %s", *recurso);
    }
}


void inicializar_vector_recursos_pedidos() {
    tam_vector_recursos_pedidos = calcular_total_instancias();
    vector_recursos_pedidos = malloc(tam_vector_recursos_pedidos * sizeof(t_recursos_pedidos));

    for (int i = 0; i < tam_vector_recursos_pedidos; i++) {
        vector_recursos_pedidos[i].PID = -1;
        vector_recursos_pedidos[i].recurso = NULL;
    }
}


int calcular_total_instancias() {
    int suma = 0;
    for (int i = 0; i < MAX_RECURSOS; i++) {
        suma += config_kernel->INST_RECURSOS[i];
    }
    return suma;
}


void finalizar_por_invalidacion(t_pcb* pcb, const char* tipo_invalidacion) {
    liberar_recurso_por_exit(pcb);
    log_info(logger, "Finaliza el proceso: %i - Motivo: %s", pcb->pid, tipo_invalidacion);
    informar_a_memoria_liberacion_proceso(pcb->pid);

    free(pcb->registros);
    free(pcb);
}


void liberar_recurso_por_exit(t_pcb* pcb) {

    for (int i = 0; i < tam_vector_recursos_pedidos; i++) {

        if (vector_recursos_pedidos[i].PID == pcb->pid) {
            int indice_recurso = obtener_indice_recurso(vector_recursos_pedidos[i].recurso);

            if (indice_recurso != -1) {
                config_kernel->INST_RECURSOS[indice_recurso]++;
            } else {
                log_error(logger, "¡Hay una descoordinación, recurso no coincide con PID: %i", pcb->pid);
                continue;
            }

            vector_recursos_pedidos[i].PID = -1;
            free(vector_recursos_pedidos[i].recurso);
            vector_recursos_pedidos[i].recurso = NULL;
        }
    }
    
    sem_post(&hay_en_estado_ready);
}