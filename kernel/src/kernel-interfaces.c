#include "kernel-interfaces.h"

sem_t semaforo_interfaces;

// Funciones de manejo de interfaz desde el lado del kernel:

void handle_new_interface(void *arg) {
    int socket_kernel = (int) arg;

    while(1) {
        int socket_cliente_nueva_interfaz = esperar_cliente("KENEL", socket_kernel);

        if(socket_cliente_nueva_interfaz != -1) {
            pthread_t hilo_nueva_interfaz;

            pthread_create(&hilo_nueva_interfaz, NULL, (void *) manage_interface, (void *) socket_cliente_nueva_interfaz);
            pthread_detach(&hilo_nueva_interfaz);
        }
    }
}

void manage_interface(void *socket_cliente) {
    int socket = (int) socket_cliente;
    int execute = 1;

    while(execute) {
        op_code operacion = recibir_operacion(socket);

        switch(operacion) {
            case NEW_INTERFACE:
                create_interface(socket);
                execute = 0;
                break;
            default:
                log_error(logger, "Operacion invalida!");
                break;
        }
    }
}

// Funciones de consumidores:

void create_consumer_thread(char *interface_name) {
    pthread_t consumer_thread;

    pthread_create(&consumer_thread, NULL, (void*) consumers_pcbs_blockeds, (void *) interface_name);
    pthread_join(consumer_thread, NULL);
}

void consumers_pcbs_blockeds(void *args) {
    sem_wait(&semaforo_interfaces);
    char *interface_name = (char *) args;
    interface_io *interface = get_interface_from_dict(interfaces, interface_name);
    sem_post(&semaforo_interfaces);

    while(1) {
        // Esperamos que la interfaz no este bloqueada:
        sem_wait(&interface->semaforo_used);
        sem_wait(&interface->size_blocked); // Esperamos que haya procesos bloqueados

        // Sacamos el primer proceso de la cola de bloqueados:
        t_pcb *pcb = queue_pop(interface->process_blocked);
        t_list *args = queue_pop(interface->args_process); // Obtenemos los argumentos del proceso

        // Obtenemos el socket de la interfaz:
        int socket_with_interface = get_socket_interface(interface);
        int response = 0;
        
        // Esto tenemos que cambiarlo, es solo para probar:
        send_message_to_interface(interface, args, &response);
        
        if(response == 1)
            mover_procesos_de_bloqueado_a_ready(pcb);

        sem_post(&interface->semaforo_used);
    }
}

// Funciones manipuladoras de interfaz:

interface_io *initialize_interface() {
    interface_io *interface = malloc(sizeof(interface_io));
    interface->socket_interface = -1;
    interface->process_blocked = queue_create();
    interface->args_process = queue_create();
    sem_init(&interface->semaforo_used, 0, 1);
    sem_init(&interface->size_blocked, 0, 0);

    return interface;
}

void create_interface(int socket) {
    tipo_interfaz tipo;
    char *interface_name;

    // Inicializamos interfaz:
    interface_io *interface = initialize_interface();

    // Recibimos el nombre de la interfaz y el tipo:
    recibir_interfaz(&interface_name, &tipo, socket);

    // Seteamos nombre de la interfaz:
    set_name_interface(interface, interface_name);

    // Seteamos las operaciones validas:
    set_valid_operations(interface, tipo);

    // Seteamos socket de la interfaz:
    set_socket_interface(interface, socket);

    // Agregamos interfaz al dccionario:
    sem_wait(&semaforo_interfaces);
    add_interface_to_dict(interface_name);
    sem_post(&semaforo_interfaces);

    create_consumer_thread(&interface_name);
}

