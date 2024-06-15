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

void peticion_IO(t_pcb *pcb_bloqueado) {
    char *interface_name;
    
    // recibimos el nombre de la interfaz:
    rcv_nombre_interfaz(&interface_name, config_kernel->SOCKET_DISPATCH);

    interface_io *interface = get_interface_from_dict(interface_name);
    tipo_interfaz tipo_de_interfaz = interface->tipo;
    tipo_operacion operacion = recibir_operacion(config_kernel->SOCKET_DISPATCH); 

    // Verificamos que la interfaz exista:
    if (!consulta_existencia_interfaz(interface) && !consulta_interfaz_para_aceptacion_de_operacion(interface)) {
        peticion_exit(pcb_bloqueado, "INVALID_INTERFACE"); // Si no existe la interfaz o no acepta la operacion, mover a exit!

        return NULL; // Salimos de la funcion
    }
    
    t_list *args = rcv_argumentos_para_io(tipo_de_interfaz); // Recibimos los argumentos de la operacion

    // Si la interfaz existe y acepta la operacion, procedemos a ejecutarla:
    queue_push(interface->process_blocked, pcb_bloqueado);
    queue_push(interface->args_process, args); // Agregamos los argumentos a la cola de argumentos
    sem_post(&interface->size_blocked); // Aumentamos el tamaño de la cola de bloqueados
}
