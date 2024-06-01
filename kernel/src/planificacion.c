#include "planificacion.h"

void inicializacion_semaforos() {
    pthread_mutex_init(&mutex_estado_ready, NULL);
    pthread_mutex_init(&mutex_estado_new, NULL);
    pthread_mutex_init(&mutex_estado_exec, NULL);
    sem_init(&habilitar_corto_plazo, 0, 0);
    sem_init(&mutex_estado_block, 0, 0);
    sem_init(&hay_en_estado_ready, 0, 0);
    sem_init(&hay_en_estado_new, 0, 0);
    sem_init(&limite_grado_multiprogramacion, 0, 10);
}


int generar_pid_unico() {
    static int pid = 0; //Es static para que mantenga su valor luego de terminar la ejecucion, y no reinicializar en 0 siempre
    return pid++;
}


void informar_a_memoria(char** archivo_de_proceso, int pid){
    char* mensaje = "Kernel envia PATH - PID";
    enviar_mensaje(mensaje, get_socket_memoria());
}


void creacion_proceso() {
    t_pcb* pcb = malloc(sizeof(t_pcb)); 
    
    if (pcb == NULL) {
        log_error(logger, "Error al asignar memoria a la creacion de PCB");
        return;
    }

    // Inicializamos
    pcb->pid = generar_pid_unico();
    pcb->registros = malloc(sizeof(t_registro_cpu));
    if (pcb->registros == NULL) {
        log_error(logger, "Error al asignar memoria a los registros del proceso");
        free(pcb);
        return;
    }

    pcb->registros->AX = 0;
    pcb->registros->BX = 0;
    pcb->registros->CX = 0;
    pcb->registros->DI = 0;
    pcb->registros->DX = 0;
    pcb->registros->EAX = 0;
    pcb->registros->EBX = 0;
    pcb->registros->ECX = 0;
    pcb->registros->EDX = 0;
    pcb->registros->PC = 0;
    pcb->registros->SI = 0;

    pcb->estado = NEW;
    pcb->program_counter = 0;
    
    log_info(logger, "Se crea el proceso %i en NEW", pcb->pid);
    agregar_a_cola_estado_new(pcb);
    informar_a_memoria("/path/.../operaciones", pcb->pid); // Notificación de creación de nuevo proceso
}


void agregar_a_cola_estado_new(t_pcb* proceso) { //NEW
    pthread_mutex_lock(&mutex_estado_new);
    list_add(cola_new, proceso);
    pthread_mutex_unlock(&mutex_estado_new);
    
    sem_post(&hay_en_estado_new);
}


void agregar_a_cola_ready() {
    while (1) {
        sem_wait(&hay_en_estado_new);
        sem_wait(&limite_grado_multiprogramacion);

        t_pcb* proceso = obtener_siguiente_a_ready();

        pthread_mutex_lock(&mutex_estado_ready);
        list_add(cola_ready, proceso);
        log_info(logger, "PID: %i - Estado Anterior: NEW - Estado Actual: READY", proceso->pid);
        pthread_mutex_unlock(&mutex_estado_ready);

        sem_post(&habilitar_corto_plazo);
    }
}


void mover_procesos_de_ready_a_bloqueado(t_pcb* proceso) {
    pthread_mutex_lock(&mutex_estado_block);
    proceso->estado = BLOCK;
    list_add(cola_block, proceso);
    log_info(logger, "PID: %i - Estado Anterior: READY - Estado Actual: BLOCK", proceso->pid);
    pthread_mutex_unlock(&mutex_estado_block);
}


void mover_procesos_de_bloqueado_a_ready(t_pcb* proceso) {
    pthread_mutex_lock(&mutex_estado_ready);
    list_add(cola_ready, proceso);
    log_info(logger, "PID: %i - Estado Anterior: BLOCK - Estado Actual: READY", proceso->pid);
    pthread_mutex_unlock(&mutex_estado_ready);
}


t_pcb* obtener_siguiente_a_ready() {
    pthread_mutex_lock(&mutex_estado_new);
    t_pcb* pcb = list_remove(cola_new, 0);
    pcb->estado = READY;
    pthread_mutex_unlock(&mutex_estado_new);
    return pcb;
}


void finalizar_proceso(t_pcb* proceso, const char* motivo){
    log_info(logger, "Proceso %i finalizado. Motivo: %s", proceso->pid, motivo);
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


// void hilo_planificador_largoplazo() {
//     pthread_t hilo_p_largo;
//     pthread_create(&hilo_p_largo, NULL, hilo_planificador_largoplazo, NULL);
//     pthread_detach(hilo_p_largo); // Detach para evitar necesidad de join, hace q los hilos sean independientes y libera los recursos una vez terminada la ejecucion del mismo
// }


// void* planificacion_largoplazo() {
//     agregar_a_cola_ready();
//     finalizar_proceso();
//     return NULL;
// }


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
        sem_wait(&habilitar_corto_plazo);
        pthread_mutex_lock(&mutex_estado_ready);
        if (!list_is_empty(cola_ready)) {
            t_pcb* proceso_actual = list_remove(cola_ready, 0);
            pthread_mutex_unlock(&mutex_estado_ready);

            proceso_actual->estado = EXEC;
            log_info(logger, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", proceso_actual->pid);
            enviar_proceso_a_cpu(proceso_actual);

            t_pcb* contexto = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);

            // Procesar el motivo de desalojo
        } else {
            pthread_mutex_unlock(&mutex_estado_ready);
            sem_post(&habilitar_corto_plazo);
        }
    }
    return NULL;
}


void* planificador_corto_plazo_RoundRobin(void* arg) {
    while (1) {
        sem_wait(&habilitar_corto_plazo);
        pthread_mutex_lock(&mutex_estado_ready);
        if (!list_is_empty(cola_ready)) {
            t_pcb* proceso_actual = list_remove(cola_ready, 0);
            pthread_mutex_unlock(&mutex_estado_ready);

            proceso_actual->estado = EXEC;
            log_info(logger, "PID: %i - Estado Anterior: READY - Estado Actual: EXEC", proceso_actual->pid);
            
            enviar_proceso_a_cpu(proceso_actual); // EN UN SENTIDO DE KERNEL A CPU

            // Crear un hilo para manejar el quantum
            pthread_t hilo_quantum; 
            if (pthread_create(&hilo_quantum, NULL, quantum_handler, (void*)proceso_actual) != 0) {
                log_error(logger, "Error al crear el hilo del quantum para el proceso %d", proceso_actual->pid);
                finalizar_proceso(proceso_actual, "Error al crear hilo del quantum");
            }

            // Recibir el contexto de ejecución, bloqueante. Cuando llega, rompe el timer
            t_pcb* contexto = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);
            if (!contexto) {
                log_error(logger, "Error al recibir el contexto de ejecución para el proceso %d", proceso_actual->pid);
                pthread_cancel(hilo_quantum);
                pthread_join(hilo_quantum, NULL);
                finalizar_proceso(proceso_actual, "Error al recibir contexto de ejecución");
            }

            // Si el proceso regresa antes de que el quantum termine, cancelamos el hilo del quantum
            pthread_cancel(hilo_quantum);
            log_info(logger, "PID: %i - Desalojado por fin de Quantum", proceso_actual->pid);
            pthread_join(hilo_quantum, NULL); // Esperar a que el hilo del quantum termine y liberar sus recursos

            pthread_mutex_lock(&mutex_estado_ready);
            list_add(cola_ready, contexto);
            pthread_mutex_unlock(&mutex_estado_ready);

        } else {
            pthread_mutex_unlock(&mutex_estado_ready);
            sem_post(&habilitar_corto_plazo);
        }
    }
    return NULL;
}

// Esto lo que estuvimos hablando:
void* quantum_handler(void* arg) {
    t_pcb* proceso = (t_pcb*)arg;
    log_info(logger, "En ejecución %d milisegundos...", config_kernel->QUANTUM);
    usleep(config_kernel -> QUANTUM * 1000); // Dormir por el tiempo del quantum

    int pid_aux = proceso->pid;
    send(config_kernel->SOCKET_INTERRUPT, &pid_aux, sizeof(int), 0); // Enviar interrupción al final del quantum

    return NULL;
}


t_pcb* rcv_contexto_ejecucion(int socket_cliente) {
    
    t_pcb* proceso = malloc(sizeof(t_pcb));
    if (proceso == NULL) {
        log_error(logger, "Error al asignar memoria para el proceso");
        return NULL;
    }

    proceso->registros = malloc(sizeof(t_registro_cpu));
    if (proceso->registros == NULL) {
        log_error(logger, "Error al asignar memoria para los registros del proceso");
        free(proceso);
        return NULL;
    }

    int size;
    int desplazamiento = 0;
    void* buffer = recibir_buffer(&size, socket_cliente);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
        free(proceso->registros);
        free(proceso);
        return NULL;
    }

    memcpy(&proceso->pid, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&proceso->program_counter, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&proceso->registros->PC, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->AX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&proceso->registros->BX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&proceso->registros->CX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&proceso->registros->DX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&proceso->registros->EAX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->EBX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->ECX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->EDX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->SI, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->DI, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    free(buffer);
    return proceso;
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

    if (cola_exit != NULL) {
        list_destroy_and_destroy_elements(cola_exit, free); // Liberar los elementos de la lista
    }

    if (cola_new != NULL) {
        list_destroy_and_destroy_elements(cola_new, free); // Liberar los elementos de la lista
    }

    if (cola_ready != NULL) {
        list_destroy_and_destroy_elements(cola_ready, free); // Liberar los elementos de la lista
    }
}



// void planificacion_VRR() {
//   
/
    

//     sem_wait(&habilitar_corto_plazo);
//     while (true){
//         //Puede haber un semaforo contador ??? para bloqueartlo y no ejecutar este if todo el tiempo
//         //Semaforo blockeo -> Si se activa guardar las cosas en cola_blockeado
//         pthread_mutex_lock(&sem_cola_ready);
//         if (list_is_empty(cola_blockeado)){
//         if (!list_is_empty(cola_ready)){
//         t_pcb* proceso_actual = list_remove(cola_ready,0);
//         pthread_mutex_unlock(&sem_cola_ready);
//             if(proceso_actual->quantum <= quantum_max){ 
//                 proceso_actual->estado = EXEC;
//                 enviar_proceso_a_cpu(proceso_actual);
//                 pthread_mutex_lock(&quantum);
//                 proceso_actual->quantum ++;
//                 proceso_actual->quantumrestante--;
//                 pthread_mutex_unlock(&quantum);
//             } else{
//                 pthread_mutex_lock(&quantum);
//                 proceso_actual->quantum = 0;
//                 pthread_mutex_unlock(&quantum);
//                 pthread_mutex_lock(&sem_cola_ready);
//                 list_add(cola_ready,proceso_actual);
//                 pthread_mutex_unlock(&sem_cola_ready);
//             }
//         } 
//         sem_wait(&habilitar_corto_plazo); 
//     }}else{
//         if(!list_is_empty(cola_blockeado)){
            
//         }
//     }
// }


// Kernel:

COMMANDS [] = {
    {IO, gestion_de_io},
}



// Esto
void* gestion_de_desalojos() {

    while(1) {

        t_pcb* contexto_nuevo = rcv_contexto_ejecucion(config_kernel->SOCKET_DISPATCH);    
        t_motivo_desalojo motivo_desalojo = recibir_desalojo();

        // Con esto re ahorras un lindo switch
        commands[motivo_desalojo].funcion(contexto_nuevo);
        (void*)(funcion(args))

        // Me gustaria hacer un diccionario con key desalojo, funcion a ejecutar
        // if(motivo_desalojo == INSTRUCCIONES_IO) {
        //     char *interface_name = recibir_mesj(socket); // Esto me da el nombre de la interfaz
        //     interface_io *interface = get_interface_from_dict(interface_name);
        //     tipo_operacion operacion = recibir_operacion(); // Esto lo tiene que mandar la CPU -> IO_SLEEEP
        //     t_pcb *pcb = rcv_contexto_ejecucion();

        //     // Tenemos que preguntar si la interfaz exista, sino hay que mandar el proceso a EXIT

        //     // Servidor le habla al cliente, y cliente le responde si o no a la operacion
        //     if(pregunto_si_acepta_operacion(interface, operacion)) { 
        //         // Me pregunto, si no necesitamos un semaforo para la cola de bloqueados
        //         sem_wait() // Si nadie esta usando la cola de bloqueados, agrego el proceso
        //         queue_push(interface->process_blocked, pcb);
        //         sem_post() // Aviso que dejo de usar la cola de bloqueados
        //         sem_post() // Aviso que hay un proceso en la cola de bloqueados

        //         // agregamos los parametros de la operacion:
        //         // Deberiamos de tener un diccionario con key PID, y la lista de parametros
        //         agregar_parametros_operacion(pcb);
        //     } else {;
        //         // Si no acepta la operacion:
        //         sem_wait() // Si nadie esta usando la cola de EXIT, agrego el proceso
        //         mover_proceso_a_exit(pcb);
        //         sem_post() // Aviso que dejo de usar la cola de EXIT
        //     }
            
     
        // }
    }
}