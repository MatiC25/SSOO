#include "planificacion.h"

t_list* cola_new = list_create();
t_list* cola_ready = list_create();
t_list* cola_block = list_create();

//PSEUDOCODE
t_pcb* creacion_proceso(t_pcb* proceso_nuevo) {
    t_config_k* config;
    informar_a_memoria(archivo_de_proceso, socket_servidor); //Notificacion de creacion de nuevo proceso
    t_pcb* pcb = malloc(sizeof(t_pcb)); 
    if(pcb == NULL) {
        log_error(logger, "Error al asignar memoria a la creacion de PCB");
        return EXIT_FAILURE;
        }

    // Inicializamos
    t_pcb* pcb = NULL;
    pcb -> pid = generar_pid_unico();
    pcb -> registro = NULL; //M
    pcb -> estado = NEW; 
    pcb -> program_counter = proceso_nuevo -> program_counter;
    log_info(logger, "Se crea el proceso <%i> en NEW", pcb->pid);
    // EL QUANTUM HABRIA QUE HACERLO GLOBAL, TE LO DEJO A VOS MI QUERIDO FEDE
}


int generar_pid_unico() {
    static int pid = 0; //Es static para que mantenga su valor luego de terminar la ejecucion, y no reinicializar en 0 siempre
    return pid++;
}


void informar_a_memoria(char** archivo_de_proceso, int socket_servidor){
    char* mensaje = "Cree un nuevo procesos";
    enviar_mensaje(mensaje, socket_servidor);
}


void agregar_a_cola_estado_new(t_pcb* proceso) { //NEW
    pthread_mutex_lock(&mutex_estado_new);
    list_add(cola_new, proceso);
    pthread_mutex_unlock(&mutex_estado_new);
    
    sem_post(&hay_en_estado_ready);
}

 
void agregar_a_cola_ready() {
    while(1) {
        sem_wait(&hay_en_estado_ready); //Se bloquea al segundo ciclo
        sem_wait(&limite_grado_multiprogramacion);
  
        t_pcb* pcb = obtener_siguiente_a_ready();
        list_add(cola_ready, pcb);

        sem_post(&habilitar_corto_plazo); 
    }
}


t_pcb* obtener_siguiente_a_ready() {
    pthread_mutex_lock(&mutex_estado_new);
    t_pcb* pcb = list_remove(cola_new, 0);
    pcb->estado = READY;
    pthread_mutex_unlock(&mutex_estado_new);
    return pcb;
}


void elegir_algoritmo_corto_plazo(char* algoritmo) {

    while(true) {
        if(strcmp(config_kernel -> algoritmo_planificacion, "FIFO") == 0){
            planificacion_fifo();
        } else if (strcmp(config_kernel -> algoritmo_planificacion, "RR")){
            planificacion_RR();
        } else if (strcmp(config_kernel -> algoritmo_planificacion, "VRR")){

        } else {
            log_error (logger,"El algoritmo no coincide con los alg")
        }
    }
    sleep(1); //Para descansar la CPU y que no se vaya al palo la cpu, que tenga una pausita (INCHEQUEABLE, pero algo hay que hacer)
}


void hilo_planificador_cortoplazo_fifo() {
    pthread_t hilo_fifo;
    pthread_create(&hilo_fifo, NULL, planificador_cortoplazo_fifo(), NULL);
    pthread_detach(hilo_fifo); // Detach para evitar necesidad de join, hace q los hilos sean independientes y libera los recursos una vez terminada la ejecucion del mismo
}


//FIFO no tiene para pasar a block simplemente por el hecho de que va de exec a ready - ready a exec (CREO)
void* planificador_cortoplazo_fifo(void* arg) {

    sem_wait(&habilitar_corto_plazo); // Esperamos que el largo plazo habilite el planificador de corto plazo

    while (true) {
        pthread_mutex_lock(&mutex_estado_ready); // Bloqueamos el acceso a la cola ready
        if (!list_is_empty(cola_ready)) {
            t_pcb* proceso_actual = list_remove(cola_ready, 0);
            pthread_mutex_unlock(&mutex_estado_ready); // Desbloqueamos
            proceso_actual->estado = EXEC;
            enviar_a_ejecutar_a_cpu(proceso_actual);
        } else {
            pthread_mutex_unlock(&mutex_estado_ready); // Si la cola_ready está vacía, desbloqueamos el acceso a la cola
            sem_wait(&habilitar_corto_plazo); // Usamos sem_wait para bloquear la espera activa y esperar nueva señal del planificador de largo plazo
        }
    }
    return NULL; // Para pthread_create, debe devolver NULL y ser de tipo void*
}


void hilo_planificador_cortoplazo_RoundRobin() { 
    pthread_t hilo_Round_Robbin;
    pthread_create(&hilo_Round_Robbin,NULL,planificador_corto_plazo_RoundRobbin(),NULL);   
    pthread_detach(hilo_Round_Robbin); // Detach para evitar necesidad de join, hace q los hilos sean independientes y libera los recursos una vez terminada la ejecucion del mismo
}


void* planificador_corto_plazo_RoundRobin(void) {

    t_config_k* config;
    int quantum_max = config->quantum;

    sem_wait(&habilitar_corto_plazo);
    while (true) {
        pthread_mutex_lock(&mutex_estado_ready);
        if (!list_is_empty(cola_ready)) {
            t_pcb* proceso_actual = list_remove(cola_ready, 0);
            pthread_mutex_unlock(&mutex_estado_ready);

            proceso_actual->estado = EXEC;
            enviar_proceso_a_cpu(proceso_actual);

            // Esperar un tiempo equivalente al quantum máximo
            usleep(quantum_max * 1000); // Convertimos el quantum a milisegundos xq usleep acepta en micro

            bool necesita_estado_block = false; //Lo pongo porque no se bien como funciona lo de una interrupcion por IO, que hace que pase a block

            if (necesita_estado_block) { //SI surge uan interrupcion que no permite completar la rafaga de quantum va a block
                mover_procesos_de_ready_a_bloqueado(proceso_actual);
            } else { //Si se desaloja por fin de quantum va a ready
                pthread_mutex_lock(&mutex_estado_ready);
                proceso->estado = READY;
                list_add(cola_ready, proceso_actual);
                pthread_mutex_unlock(&mutex_estado_ready)
            }
        } else {
            pthread_mutex_unlock(&mutex_estado_ready);
            sem_wait(&habilitar_corto_plazo);
        }
    }
    return NULL;
}

void mover_procesos_de_ready_a_bloqueado(t_pcb* proceso) {
    pthread_mutex_lock(&mutex_estado_block);
    proceso->estado = BLOCK;
    list_add(cola_block, proceso); 
    pthread_mutex_unlock(&mutex_estado_block);
}

// Todavia no se para que o como usar esto
void mover_procesos_de_bloqueado_a_ready(t_pcb* proceso) {
    pthread_mutex_lock(&mutex_estado_ready);
    proceso->estado = READY;
    list_add(cola_ready, proceso);
    pthread_mutex_unlock(&mutex_estado_ready);
}


void enviar_proceso_a_cpu(t_pcb* pcbproceso) {
    t_paquete* paquete_cpu = crear_paquete(RECIBIR_PROCESO); // Tipo de paquete que indica envío a CPU

    // Agregar información del PCB al paquete
    agregar_a_paquete(paquete_cpu, &pcbproceso->pid, sizeof(int));
    agregar_a_paquete(paquete_cpu, &pcbproceso->program_counter, sizeof(int));
    agregar_a_paquete(paquete_cpu, pcbproceso->registros, sizeof(pcbproceso->registros));
    //NO SE SI HACE FALTA ENVIARLE EL QUANTUM, SUPONGO QUE NO

    // Enviar el paquete a la CPU
    enviar_paquete(paquete_cpu, socketcpu); // socketcpu es el socket conectado a la CPU

    // Liberar recursos del paquete
    eliminar_paquete(paquete_cpu);
}


void liberar_pcb(t_pcb* estructura) {
    t_* algo = estructura -> algo;
    if( != NULL) {
        destroy_();
        estructura = NULL;
    }

    free(estructura);
}


void inicializacion_semaforos() {
    pthread_mutex_init(&mutex_estado_ready, NULL);
    pthread_mutex_init(&limite_grado_multiprogramacion, NULL);
    pthread_mutex_init(&mutex_estado_new, NULL);
    sem_init(&habilitar_corto_plazo, 0, 0);
    sem_init(&mutex_estado_block, 0, 0);
    sem_init(&hay_en_estado_ready, 0, 0);

}


// void interrupcion_bloqueante(){
//     mover_procesos_a_ready_a_bloqueado()

// }



// //Funciones Bloquiantes
// void ejecutar_wait(){
//     pthread_mutex_unlock(&cola_blockeado);

// }

// void ejecutar_singal(){
//     pthread_mutex_unlock(&cola_blockeado);

// }
// void bloque_io(){
//     pthread_mutex_unlock(&cola_blockeado);

// }
// void 

// void planificacion_VRR() {
//     t_config_k* config;
//     int quantum_max = config->quantum;
    

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
//             }else{
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

