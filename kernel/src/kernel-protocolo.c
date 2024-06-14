#include "kernel-protocolo.h"

// Funciones para enviar mensajes a interfaces:

void send_message_to_generic_interface(int socket, t_list *args, int *response) {
    char *tiempo_sleep = list_get(args, 0);
    int tiempo = atoi(tiempo_sleep);

    send(socket, &tiempo, sizeof(int), 0);
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
