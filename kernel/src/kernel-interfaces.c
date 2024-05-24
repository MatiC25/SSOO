#include "kernel-interfaces.h"

// Faltan semaforos, y ademas el dicccionario:
sem_t semaforo_diccionario = 1;

 
interface_io *initialize_interface() {
    interface_io *interface = malloc(sizeof(interface_io));
    interface->connected = 0;
    interface->process_blocked = queue_create();

    return interface;
}

void set_name_interface(interface_io *interface, char *name) {
    interface->name = name;
}

void add_interface_to_dict(interface_io *interface, char *key) { 
    dictionary_put(interfaces, key, interface);
}

interface_io *get_interface_from_dict(char *key) {
    return dictionary_get(interfaces, key);
}

void handle_new_interface(void *socket) {
    int socket_servidor = (int) socket;

    while(1) {
        pthread_t accept_interfaces_thread;
        int socket_cliente = aceptar_conexion(socket_servidor);

        pthread_create(&accept_interfaces_thread, NULL, (void*) manage_interface, (void *) socket_cliente); // Cambiado de "administrar_interfaz"
        pthread_detach(accept_interfaces_thread);
    }
}

void manage_interface(void *socket_cliente) { 
    int socket = (int *) socket_cliente;

    while(1) {
        op_code codigo_operacion = recibir_operacion(socket);

        switch(codigo_operacion) {
            case HANDSHAKE:
                generate_handshake_response(socket);
                break;
            case CREAR_INTERFAZ:
                    create_interface(socket);
        }
    }
}

void create_interface(int socket) {
    char *interface_name = recibir_mensaje(socket);
    interface_io *interface = initialize_interface();

    // Seteamos nombre de la interfaz:
    set_name_interface(interface, interface_name);

    // Agregamos interfaz al diccionario:
    sem_wait(&semaforo_diccionario);
    add_interface_to_dict(interface, interface_name);
    sem_post(&semaforo_diccionario);

    create_consumer_thread(char *interface_name);
}

void create_consumer_thread(char *interface_name) {
    pthread_t consumer_thread;

    pthread_create(&consumer_thread, NULL, (void*) producer_pcbs_blockeds, (void *) interface_name);
    pthread_join(consumer_thread, NULL);
}

void producer_pcbs_blockeds(void *args) {
    sem_wait(sem_diccionario);
    char *interface_name = (char *) args;
    interface_io *interface = get_interface_from_dict(interface_name);
    sem_post(sem_diccionario);

    while(1) {
        
        if(queue_size(interface->process_blocked) > 0) {
            int response;

            sem_wait(interface -> semaforo_blocked);
            int PID = queue_pop(interface->process_blocked); // Sacamos el primer PCB de la cola de bloqueados

            // Esperamos respuesta de la IO:
            response = send_message_to_interface(interface->SOCKET);

            if(response == 1) { // Si la IO termino de ejecutar la operacion
                sem_wait(semaforo_ready);
                cambiar_block_a_ready(PID);
                sem_post(semaforo_ready);
            } else {
                queue_push(interface->process_blocked, PID);
                sem_post(interface -> semaforo_blocked); // Si no termino, volvemos a encolar el PCB
            }
        }

        sem_post(&semaforo_diccionario);
    }
}