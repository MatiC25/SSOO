#include "kernel-peticiones-dispatch.h"

int tam_cola_resource;
t_list** colas_resource_block;
int esta_block;
int proceso_finalizado_por_consola;

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
        pthread_mutex_lock(&reanudar_ds);
        pthread_mutex_unlock(&reanudar_ds);

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
            case -1:
                log_error(logger, "El motivo de desalojo no existe!");
                return NULL;
            default:
                log_error(logger, "El motivo de desalojo no existe!");
                break;  
        }
    }
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
    proceso_en_exec = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
    pthread_mutex_unlock(&mutex_proceso_exec);
    
    if (!proceso_en_exec) {
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        return;
    }
    
    if (proceso_finalizado_por_consola == 1) {
        proceso_finalizado_por_consola = 0;
        return;
    }

    pthread_mutex_lock(&mutex_estado_ready);
    pthread_mutex_lock(&mutex_proceso_exec);
    proceso_en_exec->quantum = config_kernel->QUANTUM;
    list_add(cola_ready, proceso_en_exec);
    log_info(logger, "PID: %i - Estado Anterior: EXEC - Estado Actual: READY \n", proceso_en_exec->pid);
    pthread_mutex_unlock(&mutex_proceso_exec);
    pthread_mutex_unlock(&mutex_estado_ready);

    sem_post(&desalojo_proceso);

    log_warning(logger, "PID: %i - Desalojado por fin de Quantum", proceso_en_exec->pid);


    sem_post(&hay_en_estado_ready);
    puede_ejecutar_otro_proceso();
}


void peticion_exit(const char *tipo_de_exit) {

    pthread_mutex_lock(&mutex_proceso_exec);
    proceso_en_exec = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
    //mostrar_pcb(pcb);
    if (!proceso_en_exec) {
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        return;
    }
    pthread_mutex_unlock(&mutex_proceso_exec);

    sem_post(&desalojo_proceso);

    log_warning(logger, "Finaliza el proceso %i - Motivo: %s", proceso_en_exec->pid, tipo_de_exit);

    pthread_mutex_lock(&mutex_proceso_exec);
    proceso_en_exec->estado = EXITT;
    proceso_en_exec->quantum = 0;

    pthread_mutex_lock(&mutex_exit);
    list_add(cola_exit, proceso_en_exec);
    pthread_mutex_unlock(&mutex_exit);
    pthread_mutex_unlock(&mutex_proceso_exec);

    log_info(logger, "Se manda a Memoria para liberar el Proceso");
    informar_a_memoria_liberacion_proceso(proceso_en_exec->pid);
    log_info(logger, "Aumentamos Grado de Multiprogramacion por EXIT \n");

    pthread_mutex_lock(&mutex_proceso_exec);
    liberar_recurso_por_exit(proceso_en_exec);
    pthread_mutex_unlock(&mutex_proceso_exec);
    puede_ejecutar_otro_proceso();
}


void peticion_wait() {

    char* recurso;
    pthread_mutex_lock(&mutex_proceso_exec);
    proceso_en_exec = recibir_contexto_y_recurso(&recurso);
    if (!proceso_en_exec) {
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        puede_ejecutar_otro_proceso();
        return;
    }
    pthread_mutex_unlock(&mutex_proceso_exec);
    
    if (!recurso_existe(recurso)) {
        log_error(logger, "El recurso solicitado no existe!");
        finalizar_por_invalidacion(proceso_en_exec, "INVALID_RESOURCE");
        free(recurso);
        puede_ejecutar_otro_proceso();
        return;
    }
    
    int indice_recurso = obtener_indice_recurso(recurso);
    
    if (config_kernel->INST_RECURSOS[indice_recurso] > 0) {
        config_kernel->INST_RECURSOS[indice_recurso]--;
        sem_post(&desalojo_proceso);

        // Guardar el pid y el recurso en el vector
        for (int i = 0; i < tam_vector_recursos_pedidos; i++) {
            if (vector_recursos_pedidos[i].PID == -1) {
                vector_recursos_pedidos[i].PID = proceso_en_exec->pid;
                vector_recursos_pedidos[i].recurso = strdup(recurso);
                log_facu(logger, "PID: %i - Asignado: %s", proceso_en_exec->pid, recurso, i);
                break;
            }
        }
        
        log_facu(logger, "¡WAIT exitoso!\n");
        int wait_exitoso = 1;
        send(config_kernel->SOCKET_DISPATCH, &wait_exitoso, sizeof(int),  MSG_WAITALL);

    } else {
        log_leo(logger, "Recurso no disponible actualmente");
        log_leo(logger, "Moviendo proceso a BLOCK");

        int wait_fallido = 0;
        send(config_kernel->SOCKET_DISPATCH, &wait_fallido, sizeof(int),  MSG_WAITALL);
        
        pthread_mutex_lock(&mutex_proceso_exec);
        proceso_en_exec->quantum = config_kernel->QUANTUM;
        pthread_mutex_unlock(&mutex_proceso_exec);

        sem_post(&desalojo_proceso);
        esta_block = 1;
        pthread_mutex_lock(&mutex_proceso_exec);
        if(strcmp(config_kernel->ALGORITMO_PLANIFICACION, "VRR") == 0) {
            sem_wait(&sem_vrr);
            pthread_mutex_lock(&mutex_proceso_exec);
            proceso_en_exec->quantum = quantum_restante;
            pthread_mutex_unlock(&mutex_proceso_exec);
        }
        mover_a_bloqueado_por_wait(proceso_en_exec, recurso);
        pthread_mutex_unlock(&mutex_proceso_exec);

        puede_ejecutar_otro_proceso();
        free(recurso);
        return;
    }

    free(recurso);
}


void peticion_signal() {
    int signal_fallido = 0;
    char* recurso;
    pthread_mutex_lock(&mutex_proceso_exec);
    proceso_en_exec = recibir_contexto_y_recurso(&recurso);
    if (!proceso_en_exec) {
        log_error(logger, "Dispatch acaba de recibir algo inexistente!");
        send(config_kernel->SOCKET_DISPATCH, &signal_fallido, sizeof(int), MSG_WAITALL);
        puede_ejecutar_otro_proceso();
        return;
    }
    pthread_mutex_unlock(&mutex_proceso_exec);
    sem_post(&desalojo_proceso);

    if (!recurso_existe(recurso)) {
        log_error(logger, "El recurso solicitado no existe!");
        finalizar_por_invalidacion(proceso_en_exec, "INVALID_RESOURCE");
        send(config_kernel->SOCKET_DISPATCH, &signal_fallido, sizeof(int), MSG_WAITALL);
        free(recurso);
        puede_ejecutar_otro_proceso();
        return;
    }

    // for(int i = 0; i < tam_vector_recursos_pedidos; i++) {
    //     log_info(logger, "pid: %i",vector_recursos_pedidos[i].PID);
    //     log_info(logger, "rec: %s", vector_recursos_pedidos[i].recurso);
    // }

    int indice_recurso = obtener_indice_recurso(recurso);
    config_kernel->INST_RECURSOS[indice_recurso]++;
    // Liberar el pid y el recurso en el vector
    for (int i = 0; i < tam_vector_recursos_pedidos; i++) {
        if (vector_recursos_pedidos[i].PID == proceso_en_exec->pid && strncmp(vector_recursos_pedidos[i].recurso, recurso, 2) == 0) {
            log_facu(logger, "Proceso: %i - Devuelve: %s", proceso_en_exec->pid, recurso);
            vector_recursos_pedidos[i].PID = -1;
            free(vector_recursos_pedidos[i].recurso);
            vector_recursos_pedidos[i].recurso = NULL;
        }
    }

    log_facu(logger, "¡SIGNAL exitoso!");
    int signal_exitoso = 1;
    send(config_kernel->SOCKET_DISPATCH, &signal_exitoso, sizeof(int), MSG_WAITALL);
    

    if (!list_is_empty(colas_resource_block[indice_recurso])) {
        pthread_mutex_lock(&mutex_estado_block);
        t_pcb* pcb_signal = list_remove(colas_resource_block[indice_recurso], 0);
        pthread_mutex_unlock(&mutex_estado_block);
        log_info(logger, "¡Desbloqueando Proceso por Recurso!");
        mover_procesos_de_bloqueado_a_ready(pcb_signal);
        //liberar_pcb(pcb_signal);
    }

    free(recurso);
    //sem_post(&pedidos);
}


void liberar_pcb(t_pcb* pcb){
  if (pcb != NULL) {
        if (pcb->registros != NULL) {
            if (pcb->registros != NULL) {
                free(pcb->registros); // Liberar el arreglo dentro de Registros
            }
        }
        free(pcb); // Liberar la estructura PCB
    }
}


void mover_a_bloqueado_por_wait(t_pcb* pcb, char* recurso) {

    int indice_recurso = obtener_indice_recurso(recurso);

    pthread_mutex_lock(&mutex_estado_block);
    pcb->estado = BLOCK;
    list_add(colas_resource_block[indice_recurso], pcb);
    pthread_mutex_unlock(&mutex_estado_block);

    mover_a_cola_block_general(pcb, recurso);
}


void mover_a_cola_block_general(t_pcb* pcb, char* motivo) {

    pthread_mutex_lock(&mutex_cola_block);
    log_leo(logger, "PID: %i - Bloqueado por: %s\n", pcb->pid, motivo);
    log_info(logger, "PID: %i - Estado Anterior: EXEC - Estado Actual: BLOCK", pcb->pid);
    //log_facu(logger, "Quantum en Block: %i", pcb->quantum);
    pcb->estado = BLOCK;
    list_add(cola_block, pcb);
    pthread_mutex_unlock(&mutex_cola_block);

    sem_post(&hay_proceso_en_bloq);
}


int obtener_indice_recurso(char* recurso) {
    for (int i = 0; i < string_array_size(config_kernel->RECURSOS); i++) {
        if (strncmp(config_kernel->RECURSOS[i], recurso, 2) == 0) { //ACORDATE DE CAMBIARLOOOO!!! TODO
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
    
    pthread_mutex_lock(&mutex_proceso_exec);
    proceso_en_exec = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
    pthread_mutex_unlock(&mutex_proceso_exec);

    log_warning(logger, "¡Peticion I/O, desalojando Proceso: %i", proceso_en_exec->pid);

    pthread_mutex_lock(&mutex_proceso_exec);
    proceso_en_exec->quantum = config_kernel->QUANTUM;
    pthread_mutex_unlock(&mutex_proceso_exec);

    sem_post(&desalojo_proceso);
    esta_block = 1;
    if (!proceso_en_exec) {
        log_error(logger, "Error al recibir el contexto de ejecución para IO");
        return;
    }

    // Enviamos un mensaje de confirmación al Dispatch:
    int response = 1;
    send(config_kernel->SOCKET_DISPATCH, &response, sizeof(int), 0);
    
    // Recibimos la interfaz y los argumentos:
    int pid = proceso_en_exec->pid;
    t_list *interfaz_y_argumentos = recv_interfaz_y_argumentos(config_kernel->SOCKET_DISPATCH, pid);
    
    // Obtenemos los argumentos:
    tipo_operacion *operacion = list_get(interfaz_y_argumentos, 0);
    char *nombre_interfaz = list_get(interfaz_y_argumentos, 1); 
    t_list *args = list_get(interfaz_y_argumentos, 2);
    
    log_info(logger, "Operación: %i - Interfaz: %s", *operacion, nombre_interfaz);

    // Obtenemos la interfaz:
    interface_io* interface = get_interface_from_dict(nombre_interfaz);
    log_error(logger, "LLEGO?");

    // Verificamos si la interfaz existe y si acepta la operación:
    if (!interface) {
        finalizar_por_invalidacion(proceso_en_exec, "INVALID_INTERFACE");
        free(nombre_interfaz);
        return;
    }
    log_error(logger, "LLEGO?");

    if (!acepta_operacion_interfaz(interface, *operacion)) {
        finalizar_por_invalidacion(proceso_en_exec, "INVALID_INTERFACE");
        free(nombre_interfaz);
        return;
    }
    log_error(logger, "LLEGO?");

    if(!interface->esta_conectado) {
        finalizar_por_invalidacion(proceso_en_exec, "DISCONNECTED_INTERFACE");
        free(nombre_interfaz);
        return;
    }
    log_error(logger, "LLEGO?");

    if(strcmp(config_kernel->ALGORITMO_PLANIFICACION, "VRR") == 0) {
        sem_wait(&sem_vrr);
        log_error(logger, "ADSAD");
        pthread_mutex_lock(&mutex_proceso_exec);
        proceso_en_exec->quantum = quantum_restante;
        pthread_mutex_unlock(&mutex_proceso_exec);
    }
    log_error(logger, "LLEGO?");
    log_info(logger, "Interfaz: %s - PID: %i", nombre_interfaz, proceso_en_exec->pid);

    pthread_mutex_lock(&mutex_proceso_exec);
    mover_a_cola_block_general(proceso_en_exec, "INTERFAZ");
    pthread_mutex_unlock(&mutex_proceso_exec);

    // Agregamos el proceso a la cola de bloqueados:
    queue_push(interface->process_blocked, proceso_en_exec);
    queue_push(interface->args_process, args);
    sem_post(&interface->size_blocked);

    // Liberamos recursos:
    puede_ejecutar_otro_proceso();

}


void rcv_nombre_recurso(char** recurso, int socket) {
    op_code code = recibir_operacion(socket);
    int desplazamiento = 0;;
    int size;
    int tamanio = 0;

    void* buffer = recibir_buffer(&size, socket);
        if(buffer == NULL){
            log_error(logger, "Error al recibir el bufer en WAIT");
            return;
        }
        
        memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);

        *recurso = malloc(tamanio + 1);
    
        memcpy(*recurso, buffer + desplazamiento, tamanio);
        desplazamiento += tamanio + 1;
          
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
    for (int i = 0; i < string_array_size(config_kernel->RECURSOS); i++) {
        suma += config_kernel->INST_RECURSOS[i];
    }
    return suma;
}


int inicializar_tam_cola_resources() {
    tam_cola_resource = string_array_size(config_kernel->RECURSOS);
    //log_facu(logger, "%i", tam_cola_resource);
    return tam_cola_resource;
}


void inicializar_cola_resource_block() {
    tam_cola_resource = inicializar_tam_cola_resources();
    
    // Asignar memoria para el array de punteros a colas
    colas_resource_block = malloc(tam_cola_resource * sizeof(t_queue*));
    if (colas_resource_block == NULL) {
        // Manejar error de asignación de memoria
        log_error(logger, "No se pudo asignar memoria para colas_resource_block");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < tam_cola_resource; i++) {
        colas_resource_block[i] = list_create();
    }
}


void finalizar_por_invalidacion(t_pcb* pcb, const char* tipo_invalidacion) {
    
    log_warning(logger, "Finaliza el proceso: %i - Motivo: %s", pcb->pid, tipo_invalidacion);

    pthread_mutex_lock(&mutex_exit);
    pcb->estado = EXITT;
    list_add(cola_exit, pcb);
    pthread_mutex_unlock(&mutex_exit);

    informar_a_memoria_liberacion_proceso(pcb->pid);

    liberar_recurso_por_exit(pcb);
}


void liberar_recurso_por_exit(t_pcb* pcb) {
    for (int i = 0; i < tam_vector_recursos_pedidos; i++) {
        if (vector_recursos_pedidos[i].PID == pcb->pid) {
            int indice_recurso = obtener_indice_recurso(vector_recursos_pedidos[i].recurso);

            if (indice_recurso != -1) {
                config_kernel->INST_RECURSOS[indice_recurso]++;
                log_info(logger, "Liberado %s - PID: %i", vector_recursos_pedidos[i].recurso, pcb->pid);

                if (!list_is_empty(colas_resource_block[indice_recurso])) {
                    pthread_mutex_lock(&mutex_estado_block);
                    t_pcb* pcb_bloqueado = list_remove(colas_resource_block[indice_recurso], 0);
                    pthread_mutex_unlock(&mutex_estado_block);

                    mover_procesos_de_bloqueado_a_ready(pcb_bloqueado);
                }
            } else {
                log_error(logger, "¡Hay una descoordinación, recurso no coincide con PID: %i", pcb->pid);
                continue;
            }

            vector_recursos_pedidos[i].PID = -1;
            free(vector_recursos_pedidos[i].recurso);
            vector_recursos_pedidos[i].recurso = NULL;
        }
    }
    //puede_ejecutar_otro_proceso();
    sem_post(&sem_multiprogramacion);
}


void mostrar_pcb(t_pcb* pcb){
    log_info(logger,"PID: %i", pcb->pid);
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

