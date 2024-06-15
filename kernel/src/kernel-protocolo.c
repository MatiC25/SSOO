#include "kernel-protocolo.h"

// Funciones para enviar mensajes a interfaces:

void send_message_to_interface(interface_io *interface, t_list *args, int *response) {
    switch(interface->tipo) {
        case GENERICA:
            send_message_to_generic_interface(get_socket_interface(interface), args, response);
            break;
        case STDIN:
        case STDOUT:
            send_message_to_std_interface(get_socket_interface(interface), args, response);
            break;
        case DIALFS:
            send_message_to_dialfs_interface(get_socket_interface(interface), args, response);
            break;
        default:
            // Manejo de error o caso por defecto
            break;
    }
}

void send_message_to_generic_interface(int socket, t_list *args, int *response) {
    char *tiempo_sleep = list_get(args, 0);
    int tiempo = atoi(tiempo_sleep);

    send(socket, &tiempo, sizeof(int), 0);
    recv(socket, response, sizeof(int), 0);

    return;
}

void send_message_to_std_interface(int socket, t_list *args, int *response) {
    int direccion_fisica = list_get(args, 0);
    int bytes_a_leer = list_get(args, 1);

    send(socket, &direccion_fisica, sizeof(int), 0);
    send(socket, &bytes_a_leer, sizeof(int), 0);
    recv(socket, response, sizeof(int), 0);

    return;
}

// Funciones para recibir mensajes de interfaces:

void rcv_interfaz(char **interface_name, tipo_interfaz *tipo, int socket) {
    int size = 0;
    int desplazamiento = 0;
    int tamanio;
    void *buffer = recibir_buffer(&size, socket);
    
    memcpy(tipo, buffer + desplazamiento, sizeof(tipo_interfaz));
    desplazamiento += sizeof(tipo_interfaz);
    
    rcv_nombre_interfaz(interface_name, buffer, &desplazamiento, &tamanio);
}

void rcv_nombre_interfaz(char **interface_name, int socket) {
    int tamanio;
    int desplazamiento = 0;
    int size = 0;
    void *buffer = recibir_buffer(&size, socket);
    
    recibir_nombre_interfaz(interface_name, buffer, &desplazamiento, &tamanio);
}

t_list *rcv_argumentos_para_io(tipo_interfaz tipo_de_interfaz) {
    t_list *args = list_create();

    switch(tipo_de_interfaz) {
        case GENERICA:
            rcv_argumentos_para_io_generica(args);
            break;
        case STDIN:
        case STDOUT:
            rcv_argumentos_para_io_std(args);
            break;
        default:
            // Manejo de error o caso por defecto
            break;
    }

    return args;
}

void rcv_argumentos_para_io_generica(t_list *args) {
    int size;
    int desplazamiento = 0;
    int tiempo_sleep;
    void *buffer = recibir_buffer(&size, socket);

    memcpy(&tiempo_sleep, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    list_add(args, tiempo_sleep);
}

void rcv_argumentos_para_io_std(t_list *args) {
    int size;
    int desplazamiento = 0;
    int direccion_fisica;
    int bytes_a_leer;
    void *buffer = recibir_buffer(&size, socket);

    memcpy(&direccion_fisica, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&bytes_a_leer, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    list_add(args, direccion_fisica);
    list_add(args, bytes_a_leer);
}

void rcv_nombre_interfaz(char **interface_name, void *buffer, int *desplazamiento, int *size) { 
    memcpy(*size, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    *interface_name = malloc(tamanio);
    memcpy(*interface_name, buffer + desplazamiento, tamanio);
    *desplazamiento += *size; 
}

tipo_operacion operacion_a_realizar(int socket) {
    tipo_operacion operacion;

    // Recibimos operacion a realizar desde el CPU:
    recv(socket, &operacion, sizeof(tipo_operacion), 0);

    return operacion;
}

tipo_interfaz recibir_tipo_interfaz(int socket) {
    tipo_interfaz tipo;
    
    // Recibimos tipo de interfaz desde el CPU:
    recv(socket, &tipo, sizeof(tipo_interfaz), 0);

    return tipo;
}