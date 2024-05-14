#include "planificacion.h"

t_list* cola_new = list_create();
t_list* cola_ready = list_create();



//PSEUDOCODE
t_pcb* creacion_proceso(char** archivo_de_proceso) {
    
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
    pcb -> program_counter = 0;
    pcb -> quantum = 0 //MAXIMO 2000
    
    agregar_a_cola_estado_new(pcb);

}

void informar_a_memoria(char** archivo_de_proceso, int socket_servidor){
    char* mensaje = "Cree un nuevo procesos";
    enviar_mensaje(mensaje, socket_servidor);
}


void agregar_a_cola_estado_new(t_pcb* proceso) { //NEW
    list_add(cola_new, proceso);
}
 
void mover_procesos_a_ready(int grado_multip) {
    pthread_mutex_lock(&mutex_limite); // Bloquear el mutex una vez para el bucle

    int procesos_movidos = 0; // Variable para rastrear cuántos procesos se han movido

    // Iterar sobre la cola de NEW solo si hay espacio en la cola de READY y la cola de NEW no está vacía
    while (procesos_movidos < grado_multip && !list_is_empty(cola_new) && list_size(cola_ready) < grado_multip) { 
        t_pcb* proceso = list_remove(cola_new, 0); // Eliminar el primer elemento de cola_new
        proceso->estado = READY; 
        list_add(cola_ready, proceso); // Agregar el proceso a cola_ready
        //Semaforo contador --se agrego un proceso-- para los algoritmos (FIFO, RR, VRR)
        procesos_movidos++; // Incrementar el contador de procesos movidos
    }

    pthread_mutex_unlock(&mutex_limite); // Desbloquear el mutex una vez que se hayan movido los procesos
}

void elegir_algoritmo() {
    log_info(logger, "Elegi un algoritmo FIFO, RR, VRR");
    t_config_k config_kernel;
    if( strcmp( config_kernel -> algoritmo_planificacion, "FIFO") == 0){
        planificacion_fifo();
    } else if( strcmp( config_kernel -> algoritmo_planificacion, "RR") == 0){
        planificacion_RR();
    } else if ( strcmp( config_kernel -> algoritmo_planificacion, "VRR") == 0){
        planificacion_VRR();
    } else{
        log_error(logger,"¡¡¡Ese algoritmo no existe!!!");
    }
}

void mover_procesos_a_exit(){ //EXIT
   //Borro la lista ?? Esto se sincroniza con semaforos ??
   // semaforo singal contador
}

void planificacion_fifo() {

pthread_t hilo_FIFO
pthread_create(&hilo_FIFO,NULL,FIFO,NULL);
pthread_join(hilo_FIFO,NULL); 
}

void* FIFO (void){
     
     while (true){
        //Puede a ver un semaforo contador ??? para bloqueartlo y no ejecutar este if todo el tiempo
        if (!list_is_empty(cola_ready)){
        pthread_mutex_lock(&sem_lista);
        t_pcb* proceso_actual = list_remove(cola_ready,0);
        pthread_mutex_unlock(&sem_lista);
        proceso_actual->estado = EXEC;
        enviar_proceso_a_cpu(proceso_actual);
        }
    }    

}

void planificacion_RR() { 
pthread_t hilo_RR
pthread_create(&hilo_RR,NULL,RR,NULL);
pthread_join(hilo_RR,NULL);    
}

void* RR(void){
    t_config_k* config;
    int quantum_max = config->quantum;

    while (true){
        //Puede a ver un semaforo contador ??? para bloqueartlo y no ejecutar este if todo el tiempo
        if (!list_is_empty(cola_ready)){
        pthread_mutex_lock(&sem_lista);
        t_pcb* proceso_actual = list_remove(cola_ready,0);
        pthread_mutex_unlock(&sem_lista);
            if(proceso_actual->quantum < quantum_max){
                proceso_actual->estado = EXEC;
                enviar_proceso_a_cpu(proceso_actual);
                pthread_mutex_lock(&quantum);
                proceso_actual->quantum ++;
                pthread_mutex_unlock(&quantum);
            }else{
                proceso_actual->quantum = 0;
                pthread_mutex_lock(&sem_lista);
                list_remove(cola_ready,0);
                list_add(cola_ready,proceso_actual);
                pthread_mutex_unlock(&sem_lista);
            }
        }   
    } 
}

void planificacion_VRR() {
    
}


void enviar_proceso_a_cpu(t_pcb* pcbproceso){
    t_paquete* paquete_cpu = crear_paquete(RECIBIR_PROCESO);
    agregar_a_paquete(paquete_cpu,pcbproceso->pid,sizeof(int));
    agregar_a_paquete(paquete_cpu,pcbproceso->program_counter,sizeof(int));
    agregar_a_paquete(paquete_cpu,pcbproceso->registro,sizeof(t_registro_cpu)); //Creo que no hace falta
    //agregar_a_paquete(paquete_cpu,pcbproceso->quantum,sizeof(int); Noc si hace falta

    enviar_paquete(paquete_cpu);

    eliminar_paquete(paquete_cpu);
}

int generar_pid_unico() {
    static int pid = 0; //Es static para que mantenga su valor luego de terminar la ejecucion, y no reinicializar en 0 siempre
    return pid++;
}

void liberar_pcb(t_pcb* estructura) {
    t_* algo = estructura -> algo;
    if( != NULL) {
        destroy_();
        estructura = NULL;
    }

    free(estructura);
}


inicializacion_semaforos() {
    pthread_mutex_init(&mutex_limite, NULL);
}
