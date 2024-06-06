#include "planificacion.h"

void inicializacion_semaforos() {
    pthread_mutex_init(&mutex_estado_ready, NULL);
    pthread_mutex_init(&mutex_estado_new, NULL);
    pthread_mutex_init(&mutex_estado_exec, NULL);
    pthread_mutex_init(&mutex_estado_block, NULL);
    pthread_mutex_init(&mutex_cola_priori_vrr, NULL);
    sem_init(&habilitar_corto_plazo, 0, 0);
    sem_init(&hay_en_estado_ready, 0, 0);
    sem_init(&hay_en_estado_new, 0, 0);
    sem_init(&limite_grado_multiprogramacion, 0, 10);
    sem_init(&pcb_ya_recibido, 0, 0);
}


int generar_pid_unico() {
    static int pid = 0; //Es static para que mantenga su valor luego de terminar la ejecucion, y no reinicializar en 0 siempre
    return pid++;
}


void informar_a_memoria_creacion_proceso(char* archivo_de_proceso, int pid){
    t_paquete* paquete = crear_paquete(PSEUDOCODIGO);

    agregar_a_paquete_string(paquete, archivo_de_proceso, strlen(archivo_de_proceso) + 1);
    agregar_a_paquete(paquete, pid, sizeof(pid));

    enviar_paquete(paquete, config_kernel->SOCKET_MEMORIA);
}


void creacion_proceso() {
    t_pcb* pcb = malloc(sizeof(t_pcb)); 
    
    if (pcb == NULL) {
        log_error(logger, "Error al asignar memoria a la creación de PCB");
        return;
    }

    pcb->pid = generar_pid_unico();
    pcb->registros = calloc(1, sizeof(t_registro_cpu));  // Usar calloc para inicializar los registros a 0
    if (pcb->registros == NULL) {
        log_error(logger, "Error al asignar memoria a los registros del proceso");
        free(pcb);
        return;
    }

    pcb->estado = NEW;
    pcb->program_counter = 0;
    
    log_info(logger, "Se crea el proceso %i en NEW", pcb->pid);
    agregar_a_cola_estado_new(pcb);
    informar_a_memoria_creacion_proceso("/path/.../operaciones", pcb->pid);
} 


void agregar_a_cola_estado_new(t_pcb* proceso) { //NEW
    pthread_mutex_lock(&mutex_estado_new);
    list_add(cola_new, proceso);
    pthread_mutex_unlock(&mutex_estado_new);
    
    sem_post(&hay_en_estado_new);
}


void* agregar_a_cola_ready() {
    while (1) {
        sem_wait(&hay_en_estado_new);
        sem_wait(&limite_grado_multiprogramacion);

        t_pcb* proceso = obtener_siguiente_a_ready();

        pthread_mutex_lock(&mutex_estado_ready);
        list_add(cola_ready, proceso);
        log_info(logger, "PID: %i - Estado Anterior: NEW - Estado Actual: READY", proceso->pid);
        pthread_mutex_unlock(&mutex_estado_ready);

        sem_post(&habilitar_corto_plazo);
        sem_post(&hay_en_estado_ready);
    }
    return NULL;
}


void mover_procesos_de_ready_a_bloqueado(t_pcb* proceso) {
    pthread_mutex_lock(&mutex_estado_ready);
    proceso = list_remove(cola_ready, 0);
    pthread_mutex_unlock(&mutex_estado_ready);

    pthread_mutex_lock(&mutex_estado_block);
    proceso->estado = BLOCK;
    list_add(cola_block, proceso);
    log_info(logger, "PID: %i - Estado Anterior: READY - Estado Actual: BLOCK", proceso->pid);
    pthread_mutex_unlock(&mutex_estado_block);
}


void mover_procesos_de_bloqueado_a_ready(t_pcb* proceso) {
    pthread_mutex_lock(&mutex_estado_block);
    proceso = list_remove(cola_block, 0);
    pthread_mutex_unlock(&mutex_estado_block);

    pthread_mutex_lock(&mutex_estado_ready);
    list_add(cola_ready, proceso);
    log_info(logger, "PID: %i - Estado Anterior: BLOCK - Estado Actual: READY", proceso->pid);
    pthread_mutex_unlock(&mutex_estado_ready);

    sem_post(&hay_en_estado_ready);
}


t_pcb* obtener_siguiente_a_ready() {
    pthread_mutex_lock(&mutex_estado_new);
    t_pcb* pcb = list_remove(cola_new, 0);
    pcb->estado = READY;
    log_info(logger, "PID: %i - Estado Anterior: NEW - Estado Actual: READY", pcb->pid);
    pthread_mutex_unlock(&mutex_estado_new);
    return pcb;
}


void informar_a_memoria_liberacion_proceso(int pid) {
    t_paquete* paquete = crear_paquete(FINALIZAR_PROCESO);
    agregar_a_paquete(paquete, pid, sizeof(int));
    enviar_paquete(paquete, config_kernel->SOCKET_MEMORIA);
    eliminar_paquete(paquete); 
    //NO SE SI DEBERIA ESPERAR A QUE MEMORIA ME DE EL OK PARA BORRAR EL PROCESO
}


void finalizar_proceso(t_pcb* proceso, const char* motivo){
    log_info(logger, "Proceso %i finalizado. Motivo: %s", proceso->pid, motivo);

    informar_a_memoria_liberacion_proceso(proceso -> pid);

    liberar_proceso(proceso);
    sem_post(&limite_grado_multiprogramacion);
}


void liberar_proceso(t_pcb* proceso) {
    free(proceso -> registros);
    free(proceso);
}

void elegir_algoritmo_corto_plazo() {
    if(!strcmp(config_kernel->ALGORITMO_PLANIFICACION, "FIFO")) {
        hilo_planificador_cortoplazo_fifo();
    } else if(!strcmp(config_kernel->ALGORITMO_PLANIFICACION, "RR")) {
        hilo_planificador_cortoplazo_RoundRobin();
    } else if(!strcmp(config_kernel->ALGORITMO_PLANIFICACION, "VRR")) {
        // Implementar VRR
    } else {
        log_error(NULL, "El algoritmo no coincide con los algoritmos");
    }
}


void hilo_planificador_largoplazo() {
    pthread_t hilo_p_largo;
    pthread_create(&hilo_p_largo, NULL, agregar_a_cola_ready, NULL);
    pthread_detach(hilo_p_largo); // Detach para evitar necesidad de join, hace q los hilos sean independientes y libera los recursos una vez terminada la ejecucion del mismo
}


void hilo_planificador_cortoplazo_fifo() {
    pthread_t hilo_fifo;
    pthread_create(&hilo_fifo, NULL, planificador_cortoplazo_fifo, NULL);
    pthread_detach(hilo_fifo); // Detach para evitar necesidad de join, hace q los hilos sean independientes y libera los recursos una vez terminada la ejecucion del mismo
}


void hilo_planificador_cortoplazo_RoundRobin() {
    pthread_t hilo_RoundRobin;
    pthread_create(&hilo_RoundRobin, NULL, planificador_corto_plazo_RoundRobin, NULL);
    pthread_detach(hilo_RoundRobin);
}


void* planificador_cortoplazo_fifo(void* arg) {
    while (1) {
        sem_wait(&nuevo_pcb_a_ejecutar); // Una vez se gestiona el desalojo (exit, io, wait o signal)
        sem_wait(&habilitar_corto_plazo);
        sem_wait(&hay_en_estado_ready);

        pthread_mutex_lock(&mutex_estado_ready);
        t_pcb* proceso_actual = list_remove(cola_ready, 0);
        pthread_mutex_unlock(&mutex_estado_ready);

        proceso_actual->estado = EXEC;
        log_info(logger, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", proceso_actual->pid);
        enviar_proceso_a_cpu(proceso_actual);

    }
    return NULL;
}


void* planificador_corto_plazo_RoundRobin(void* arg) {
    while (1) {
        sem_wait(&nuevo_pcb_a_ejecutar);
        sem_wait(&habilitar_corto_plazo);
        sem_wait(&hay_en_estado_ready);  // Espera hasta que haya procesos en READY
        pthread_mutex_lock(&mutex_estado_ready);
        t_pcb* proceso_actual = list_remove(cola_ready, 0);  // Extraer el primer proceso en READY
        pthread_mutex_unlock(&mutex_estado_ready);

        if (proceso_actual != NULL) {
            proceso_actual->estado = EXEC;
            log_info(logger, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", proceso_actual->pid);
            enviar_proceso_a_cpu(proceso_actual);

            // Crear un hilo para manejar el quantum
            pthread_t hilo_quantum; 
            if (pthread_create(&hilo_quantum, NULL, quantum_handler, (void*)proceso_actual) != 0) {
                log_error(logger, "Error al crear el hilo del quantum para el proceso %d", proceso_actual->pid);
                finalizar_proceso(proceso_actual, "Error al crear hilo del quantum");
            }

            sem_wait(&pcb_ya_recibido); // Una vez se recibe el pcb, se habiltia a romper los hilos
            pthread_cancel(hilo_quantum);
            pthread_join(hilo_quantum, NULL);  // Esperar a que el hilo del quantum termine y liberar sus recursos

        } else {
            sem_post(&habilitar_corto_plazo);  // Rehabilitar el planificador si no se encontró un proceso
        }
    }
    return NULL;
}


void* quantum_handler(void* arg) {
    t_pcb* proceso = (t_pcb*)arg;
    log_info(logger, "En ejecución %d milisegundos...", config_kernel->QUANTUM);
    usleep(config_kernel -> QUANTUM * 1000); // Dormir por el tiempo del quantum

    int pid_aux = proceso->pid;
    send(config_kernel->SOCKET_INTERRUPT, &pid_aux, sizeof(int), 0); // Enviar interrupción al final del quantum

    return NULL;
}


// void planificacion_cortoplazo_VRR() {

//     while(1) {
//         sem_wait(&habilitar_corto_plazo);
//         sem_wait(&hay_en_estado_ready);

//         t_pcb* proceso_actual = NULL;

//         pthread_mutex_lock(&mutex_cola_priori_vrr);
//         pthread_mutex_lock(&mutex_estado_ready);
//         if(!list_is_empty(cola_prima_VRR)) {
//             proceso_actual = list_remove(cola_prima_VRR, 0);
//             pthread_mutex_lock(&mutex_cola_priori_vrr);
//         }
//         else {
//             proceso_actual = list_remove(cola_ready, 0);   
//             pthread_mutex_lock(&mutex_estado_ready);     
//         }

//         if(!proceso_actual) {

//             proceso_actual->estado = EXEC;
//             log_info(logger, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", proceso_actual->pid);
//             enviar_proceso_a_cpu(proceso_actual);

//             pthread_t hilo_quantum_vrr;
//             if(pthread_create(&hilo_quantum_vrr, NULL, quantum_vrr_handler, (void*) proceso_actual) != 0) {
//                 log_error(logger, "Error al crear el hilo del quantum para el proceso %d", proceso_actual->pid);
//                 finalizar_proceso(proceso_actual, "Error al crear hilo del quantum");
//             }

//             // t_pcb* contexto_recibido = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
//             // if(!contexto_recibido) {
//             //     log_error(logger, "Error al recibir el contexto de ejecución para el proceso %d", proceso_actual->pid);
//             //     pthread_cancel(hilo_quantum_vrr);
//             //     pthread_join(hilo_quantum_vrr, NULL);
//             //     finalizar_proceso(proceso_actual, "Error al recibir contexto de ejecución");
//             // }

//             // pthread_cancel(hilo_quantum_vrr);
//             // log_info(logger, "PID: %i - Desalojado por fin de Quantum", proceso_actual->pid);
//             // pthread_join(hilo_quantum_vrr, NULL);  // Esperar a que el hilo del quantum termine y liberar sus recursos

//         }
//     }     
// }


void* quantum_vrr_handler(void* arg) {
    t_pcb* proceso = (t_pcb*)arg;
    t_temporal* tiempo_ejecucion = temporal_create();
    
}


void enviar_proceso_a_cpu(t_pcb* pcbproceso) {
    t_paquete* paquete_cpu = crear_paquete(RECIBIR_PROCESO); // Tipo de paquete que indica envío a CPU

    // Agregar información del PCB al paquete
    agregar_a_paquete(paquete_cpu, &pcbproceso->pid, sizeof(int));
    agregar_a_paquete(paquete_cpu, &pcbproceso->program_counter, sizeof(int));

    // Agregar los registros de la CPU al paquete individualmente
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros->PC, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros->AX, sizeof(uint8_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros->BX, sizeof(uint8_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros->CX, sizeof(uint8_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros->DX, sizeof(uint8_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros->EAX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros->EBX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros->ECX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros->EDX, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros->SI, sizeof(uint32_t));
    agregar_a_paquete(paquete_cpu, &pcbproceso->registros->DI, sizeof(uint32_t));

    // Enviar el paquete a la CPU
    enviar_paquete(paquete_cpu, config_kernel->SOCKET_DISPATCH);

    // Liberar recursos del paquete
    eliminar_paquete(paquete_cpu);
}


void prevent_from_memory_leaks() {

    // Liberar las colas si existen
    if (cola_block != NULL) {
        list_destroy_and_destroy_elements(cola_block, free); // Liberar los elementos de la lista
    }

    if (cola_new != NULL) {
        list_destroy_and_destroy_elements(cola_new, free); // Liberar los elementos de la lista
    }

    if (cola_ready != NULL) {
        list_destroy_and_destroy_elements(cola_ready, free); // Liberar los elementos de la lista
    }
}



// Kernel:



            // // Recibir el contexto de ejecución, bloqueante. Cuando llega, rompe el timer
            // t_pcb* contexto = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
            // if (!contexto) {
            //     log_error(logger, "Error al recibir el contexto de ejecución para el proceso %d", proceso_actual->pid);
            //     pthread_cancel(hilo_quantum);
            //     pthread_join(hilo_quantum, NULL);
            //     finalizar_proceso(proceso_actual, "Error al recibir contexto de ejecución");
            // }