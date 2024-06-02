#include "kernel-interfaces.h"


// Funciones de manejo de interfaz desde el lado del kernel:

void handle_new_interface(void* arg) {
    int socket_servidor = (intptr_t) arg;

    while(1) {
        pthread_t accept_interfaces_thread;
        int socket_cliente = esperar_cliente("Interfaces", socket_servidor);

        pthread_create(&accept_interfaces_thread, NULL, (void*)manage_interface, (void *)(intptr_t) socket_cliente); // Cambiado de "administrar_interfaz"
        pthread_detach(accept_interfaces_thread);
    }
}

void manage_interface(void *socket_cliente) { 
    int socket = (int) socket_cliente;
    int seguir = 1;

    while(seguir) {
        op_code codigo_operacion = recibir_operacion(socket);

        switch(codigo_operacion) {
            case HANDSHAKE:
                recibir_handshake(socket);
            break;
            case CREAR_INTERFAZ:
                create_interface(socket);
                seguir = 0;
            break;
            default:
                seguir = 0;
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
    sem_wait(&sem_diccionario);
    char *interface_name = (char *) args;
    interface_io *interface = get_interface_from_dict(interface_name);
    sem_post(&sem_diccionario);

    while(1) {
        
        if(queue_size(interface->process_blocked) > 0) {
            int response;
            t_pcb* proceso = queue_pop(interface->process_blocked); // Sacamos el primer PCB de la cola de bloqueados
            

            // Esperamos respuesta de la IO:
            response = send_message_to_interface(interface->SOCKET);

            if(response == 1) { // Si la IO termino de ejecutar la operacion
                cambiar_block_a_ready(proceso);
            } else {
                queue_push(interface->process_blocked,proceso);
            }
        }
    }
}

// Funciones manipuladoras de interfaz:

interface_io *initialize_interface() {
    interface_io *interface = malloc(sizeof(interface_io));
    interface->connected = 0;
    interface->SOCKET = -1;
    interface->process_blocked = queue_create();
    sem_init(&interface->semaforo_blocked, 0, 1);

    return interface;
}

void create_interface(int socket) {
    char *interface_name = recibir_mesj(socket);
    interface_io *interface = initialize_interface();

    // Seteamos nombre de la interfaz:
    set_name_interface(interface, interface_name);

    // Agregamos interfaz al diccionario:
    sem_wait(&semaforo_diccionario);
    add_interface_to_dict(interface, interface_name);
    sem_post(&semaforo_diccionario);

    create_consumer_thread(&interface_name);
}

// Metodos de interfaz:

void set_name_interface(interface_io *interface, char *name) {
    interface->name = name;
}

int get_socket_interface(interface_io *interface) {
    return interface->SOCKET;
}

void add_interface_to_dict(interface_io *interfaces, char *key) { 
    dictionary_put(interface, key, interfaces);
}

interface_io *get_interface_from_dict(char *key) {
    return dictionary_get(interface, key); 
}

// Funciones para pedir operaciones a la interfaz:

int consulta_interfaz_para_aceptacion_de_operacion(interface_io *interface, tipo_operacion operacion) {
    int result;
    int socket_interface = get_socket_interface(interface);

    // Enviamos operacion a la interfaz:
    send(socket_interface, &operacion, sizeof(tipo_operacion), 0);
    recv(socket_interface, &result, sizeof(int), 0); // Recibimos respuesta de la interfaz

    return result;
}

consulta_existencia_interfaz(interface_io *interface) {
    return interface != NULL;
}