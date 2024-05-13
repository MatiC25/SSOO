#include "planificacion.h"

t_list* cola_new;
t_list* cola_ready;



//PSEUDOCODE
t_pcb* creacion_proceso(char** archivo_de_proceso, int quantum) {
    
    recibir_solicitud_consola(); //Recibir solicitud de creación de proceso por consola
    informar_a_memoria(archivo_de_proceso, ); //Notificacion de creacion de nuevo proceso

    // t_pcb* pcb = malloc(sizeof(t_pcb)); POR LO QUE ENTIENDO MEMORIA ES EL Q RESERVA LA MEMORIA, POR ESO COMENTO
    // if(pcb == NULL) {
    //     log_error(logger, "Error al asignar memoria a la creacion de PCB");
    //     return EXIT_FAILURE;
    // } 

    // Inicializamos
    t_pcb* pcb = NULL;
    pcb -> pid = generar_pid_unico();
    pcb -> registro = NULL; //M
    pcb -> estado = NEW; 
    pcb -> tabla_paginacion = NULL;
    pcb -> program_counter = 0;
    pcb -> quantum = quantum; //2000
    
    agregar_a_cola_estado_new(pcb);

}

void agregar_a_cola_estado_new(t_pcb* proceso) { //NEW
    
    list_add(cola_new, proceso);
}
 
void mover_procesos_a_ready(int grado_multip) { //READY
    for(int i = 0; i < grado_multip && !list_is_empty(cola_new); i++) { //NO es un solo proceso?
        t_pcb* proceso = list_remove(cola_new, i);
        proceso -> estado = READY; //estas rescribiendo muchas veces 
        list_add(cola_ready, proceso);
    }
    elegir_algoritmo();

}

void elegir_algoritmo(){
          if(t_config_k -> algoritmo_planificacion == "FIFO"){
        planificacion_fifo();
    } else if(t_config_k -> algoritmo_planificacion == "RR"){
        planificacion_RR();
    } else if (t_config_k -> algoritmo_planificacion == "VRR"){
        planificacion_VRR();
    } else{
        log_error(logger,"Ese algoritmo no existe")
    }
}

void mover_procesos_a_exit(){ //EXIT
   //Borro la lista ?? Esto se sincroniza con semaforos ??
}

void planificacion_fifo() {

    t_pcb* pcbReady = list_remove(cola_ready,0); //Noc como hacer el for si es qeu es mas de un proceso no puedo cambiar el estado
    
    pcbReady->estado = EXEC;
    ejecutar_proceso(pcbReady);

    
}

void planificacion_RR() { //NOC si esta bien creo que 
int quantumllevado;
t_pcb* pcbRR;

    if (quantumllevado <  pcbRR->quantum){
   //MUTEX
   quantumllevado ++;
    //MUTEX
    t_pcb* pcbReady = list_remove(cola_ready,0); //Noc como hacer el for si es qeu es mas de un proceso no puedo cambiar el estado
    
    pcbReady->estado = EXEC;
    ejecutar_proceso(pcbReady);

    }else{
        //poner en blockeado por una rafaga de otro proceso 
    }
    

}

void planificacion_VRR() {
    
}


void enviar_proceso_a_cpu(t_pcb* pcbproceso){
    t_paquete* paquete_cpu = crear_paquete(RECIBIR_PROCESO);
    agregar_a_paquete(paquete_cpu,pcbproceso->pid,sizeof(int));
    agregar_a_paquete(paquete_cpu,pcbproceso->program_counter,sizeof(int));
    agregar_a_paquete(paquete_cpu,pcbproceso->registro,sizeof(t_registro_cpu));
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



