#include "kernel-peticiones-dispatch.h"

void hilo_motivo_de_desalojo() {
    pthread_t hilo_desalojo;

    // Crear el hilo de desalojo y verificar si hay errores
    if (pthread_create(&hilo_desalojo, NULL, escuchar_peticiones_dispatch, NULL) != 0) {
        log_error(logger, "ROMPIO HILO DE DESALOJO");
        return;
    }

    log_info(logger, "Hilo de desalojo creado con éxito!");

    // Detach el hilo para liberar recursos automáticamente cuando el hilo termine
    if (pthread_detach(hilo_desalojo) != 0) {
        log_error(logger, "No se pudo detach el hilo de desalojo");
    }
}


void* escuchar_peticiones_dispatch() {
    while (1) {
        
        t_tipo_instruccion motivo_desalojo = recibir_operacion(config_kernel->SOCKET_DISPATCH);

        //op_code motivo_desalojo;
        //recv(config_kernel->SOCKET_DISPATCH, &motivo_desalojo, sizeof(int), MSG_WAITALL);

        if (motivo_desalojo < 0) {
            log_error(logger, "Dispatch acaba de recibir un motivo de desalojo inválido!");
            continue;
        }

        const char* tipo_de_exit = transformar_motivos_a_exit(&motivo_desalojo);

        log_info(logger, "Motivo de desalojo: %i", motivo_desalojo);

        switch (motivo_desalojo) {
            case FIN_QUANTUM:
                peticion_fin_quantum();
                break;
            case OPERACION_IO:
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
            *motivo_inicial = EXIT;
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


void peticion_exit(const char *tipo_de_exit) {

    t_pcb* pcb = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
    log_info(logger, "PID: %i", pcb->pid);
    log_info(logger, "Program Counter: %i", pcb->program_counter);
    log_info(logger, "PC: %i", pcb->registros->PC);
    log_info(logger, "AX: %i", pcb->registros->AX);
    log_info(logger, "BX: %i", pcb->registros->BX);
    log_info(logger, "CX: %i", pcb->registros->CX);
    log_info(logger, "DX: %i", pcb->registros->DX);
    log_info(logger, "EAX: %i", pcb->registros->EAX);
    log_info(logger, "EBX: %i", pcb->registros->EBX);
    log_info(logger, "ECX: %i", pcb->registros->ECX);
    log_info(logger, "EDX: %i", pcb->registros->EDX);
    log_info(logger, "SI: %i", pcb->registros->SI);
    log_info(logger, "DI: %i", pcb->registros->DI);
    if (!pcb) {
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        return;
    }

    //const char *tipo_de_exit = transformar_motivos_a_exit(tipo_de_exit);

    log_info(logger, "Finaliza el proceso %i - Motivo: %s", pcb->pid, tipo_de_exit);

    // Verificar si el proceso tiene algún recurso asociado
    liberar_recurso_por_exit(pcb);
    
    log_info(logger, "Se manda a Memoria para liberar el Proceso");
    // informar_a_memoria_liberacion_proceso(pcb->pid);
    log_info(logger, "Aumentamos Grado de Multiprogramacion por EXIT");
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

    int indice_recurso = obtener_indice_recurso(recurso);

    pthread_mutex_lock(&mutex_estado_block);
    pcb->estado = BLOCK;
    queue_push(colas_resource_block[indice_recurso], pcb);
    pthread_mutex_unlock(&mutex_estado_block);

    mover_a_cola_block_general(pcb, recurso);

}


void mover_a_cola_block_general(t_pcb* pcb, char* motivo) {
    log_info(logger, "PID: %i - Estado Anterior: EXEC - Estado Actual: BLOCK", pcb->pid);

    pthread_mutex_lock(&mutex_cola_block);
    log_info(logger, "PID: %i - Bloqueado por: %s", pcb->pid, motivo);
    list_add(cola_block, pcb);
    pthread_mutex_unlock(&mutex_cola_block);
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
    
    int response = 1;

    send(config_kernel->SOCKET_DISPATCH, &response, sizeof(int), 0);
    
    t_list *interfaz_y_argumentos = recv_interfaz_y_argumentos(config_kernel->SOCKET_DISPATCH);

    tipo_operacion *operacion = list_get(interfaz_y_argumentos, 0);
    t_list *args = list_get(interfaz_y_argumentos, 1);
    char *nombre_interfaz = list_get(interfaz_y_argumentos, 2);
    
    interface_io* interface = get_interface_from_dict(nombre_interfaz);

    if (!interface) {
        finalizar_por_invalidacion(pcb_bloqueado, "INVALID_INTERFACE");
        free(nombre_interfaz);
        return;
    }

    // // tipo_operacion operacion = recibir_operacion(config_kernel->SOCKET_DISPATCH);
    if (!acepta_operacion_interfaz(interface, *operacion)) {
        finalizar_por_invalidacion(pcb_bloqueado, "INVALID_INTERFACE");
        free(nombre_interfaz);
        return;
    }

    // // t_list* args = rcv_argumentos_para_io(interface->tipo, config_kernel->SOCKET_DISPATCH);

    // for(int i = 0; i < list_size(args); i++) {
    //     log_info(logger, "Argumento %i: %i", i, list_get(args, i));
    // }
    
    mover_a_cola_block_general(pcb_bloqueado, "INTERFAZ");

    queue_push(interface->process_blocked, pcb_bloqueado);
    queue_push(interface->args_process, args);
    sem_post(&interface->size_blocked);
    // free(interface_name);
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
    
    log_info(logger, "Finaliza el proceso: %i - Motivo: %s", pcb->pid, tipo_invalidacion);
    informar_a_memoria_liberacion_proceso(pcb->pid);
    liberar_recurso_por_exit(pcb);
    
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
    free(pcb->registros);
    free(pcb);
    
    sem_post(&hay_en_estado_ready);
}

void mostrar_pcb(t_pcb* pcb){
    log_info(logger,"PID: %i", pcb->pid);
    log_info(logger,"Program Counter:%i",pcb->program_counter);
    log_info(logger,"Reg PC:%i",pcb->registros->PC);
    log_info(logger,"Reg AX:%i",pcb->registros->AX);
    log_info(logger,"Reg BX:%i",pcb->registros->BX);
    log_info(logger,"Reg CX:%i",pcb->registros->CX);
    log_info(logger,"Reg DX:%i",pcb->registros->DX);
    log_info(logger,"Reg EAX:%i",pcb->registros->EAX);
    log_info(logger,"Reg EBX:%i",pcb->registros->EBX);
    log_info(logger,"Reg ECX:%i",pcb->registros->ECX);
    log_info(logger,"Reg EDX:%i",pcb->registros->EDX);
    log_info(logger,"Reg SI:%i",pcb->registros->SI);
    log_info(logger,"Reg DI:%i",pcb->registros->DI);
}