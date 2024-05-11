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

mover_procesos_a_ready(int grado_multip) {
    for(int i = 0; i < grado_multip && !list_is_empty(cola_new); i++) {
        t_pcb* proceso = list_remove(cola_new, i);
        proceso -> estado = READY;
        list_add(cola_ready, proceso);
    }
}

void planificacion_fifo() {
    
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

void agregar_a_cola_estado_new(t_pcb* proceso) {
    list_add(cola_new, proceso);
}

enviar_proceso_a_cpu() {

}