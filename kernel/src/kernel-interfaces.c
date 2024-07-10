#include "kernel-interfaces.h"

sem_t semaforo_interfaces;

// Funciones de manejo de interfaz desde el lado del kernel:

void handle_new_interface(void *arg) {
    int socket_kernel = (int) arg;
    sem_init(&semaforo_interfaces, 0, 1);
    inicializar_diccionario_interfaces();

    while(1) {
        int socket_cliente_nueva_interfaz = esperar_cliente("KENEL", socket_kernel);

        if(socket_cliente_nueva_interfaz != -1) {
            pthread_t hilo_nueva_interfaz;

            pthread_create(&hilo_nueva_interfaz, NULL, (void *) manage_interface, (void *) socket_cliente_nueva_interfaz);
            pthread_detach(hilo_nueva_interfaz);
        }
    }
}

void manage_interface(void *socket_cliente) {
    int socket = (int) socket_cliente;
    int execute = 1;

    while(execute) {
        op_code operacion = recibir_operacion(socket);

        //log_info(logger, "Operacion recibida: %d", operacion);

        switch(operacion) {
            case HANDSHAKE:
                recibir_handshake(socket);
                break;
            case CREAR_INTERFAZ:
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

    // Crear un hilo consumidor y manejar cualquier error de creación
    if (pthread_create(&consumer_thread, NULL, consumers_pcbs_blockeds, (void *) interface_name) != 0) {
        perror("Error al crear el hilo consumidor");
        exit(EXIT_FAILURE);
    }
    pthread_detach(consumer_thread);
}

void consumers_pcbs_blockeds(void *args) {
    char *interface_name = (char *) args;
    interface_io *interface = get_interface_from_dict(interface_name);

    while(1) {
        sem_wait(&interface->semaforo_used);
        sem_wait(&interface->size_blocked);

        t_pcb *pcb = queue_pop(interface->process_blocked);
        t_list *args_pcb = queue_pop(interface->args_process);

        int socket_with_interface = get_socket_interface(interface);
        int response = 0;
        
        int buffer;
        int sigue_conectado = recv(socket_with_interface, &buffer, sizeof(int), MSG_PEEK | MSG_DONTWAIT);

        if (sigue_conectado == 0) {
            interface->esta_conectado = 0;  
            log_warning(logger, "Interfaz: %s - Desconectada", interface_name);

            sem_post(&interface->size_blocked);
            queue_push(interface->process_blocked, pcb);
            queue_push(interface->args_process, args_pcb);

            break; // Cambiar return a break para salir del loop y limpiar
        }

        log_info(logger, "Proceso: %d - Enviando a interfaz: %s", pcb->pid, interface_name);
        send_message_to_interface(interface, args_pcb, &response, socket_with_interface);

        if (response == 1) {
            log_warning(logger, "Proceso: %d - Terminado por: %s", pcb->pid, interface_name);
            mover_procesos_de_bloqueado_a_ready(pcb);
        }

        sem_post(&interface->semaforo_used);
        list_destroy_and_destroy_elements(args_pcb, free);
        //liberar_pcb(pcb); 
    }

    // Limpiar recursos cuando se desconecta
    pthread_exit(NULL); // Finaliza el hilo limpiamente
}

// Funciones manipuladoras de interfaz:

interface_io *initialize_interface() {
    interface_io *interface = malloc(sizeof(interface_io));
    interface->socket_interface = -1;
    interface->process_blocked = queue_create();
    interface->args_process = queue_create();
    sem_init(&interface->semaforo_used, 0, 1);
    sem_init(&interface->size_blocked, 0, 0);
    interface->esta_conectado = 1;

    return interface;
}

void create_interface(int socket) {
    tipo_interfaz tipo;
    char *interface_name;

    interface_io *interface = initialize_interface();
    rcv_interfaz(&interface_name, &tipo, socket);

    sem_wait(&semaforo_interfaces);
    if(!esta_interfaz(interface_name)) {
    sem_post(&semaforo_interfaces);
        log_info(logger, "Nueva interfaz conectada: %s", interface_name);
        set_name_interface(interface, interface_name);
        set_tipo_interfaz(interface, tipo);
        set_socket_interface(interface, socket);

        sem_wait(&semaforo_interfaces);
        add_interface_to_dict(interface, interface_name);
        sem_post(&semaforo_interfaces);

        create_consumer_thread(interface_name);
    } else {
        interface_io *interface_old = get_interface_from_dict(interface_name);

        if(!interface_old->esta_conectado) {
            log_info(logger, "Interfaz conectada de nuevo: %s", interface_name);

            interface_old->esta_conectado = 1;    

            set_socket_interface(interface_old, socket);
            sem_post(&interface_old->semaforo_used);
            create_consumer_thread(interface_name);
        } else 
            close(socket);
    }
}