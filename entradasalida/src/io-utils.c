#include "io-utils.h"

//  Setea el socket con la memoria:
void set_socket_memory(int socket, t_interfaz * interfaz) {
    interfaz->socket_with_memoria = socket;
}

// Devuelve el socket con la memoria:
int get_socket_memory(t_interfaz * interfaz) {
    return interfaz->socket_with_memoria;
}

// Setea el socket con el kernel:
void set_socket_kernel(int socket, t_interfaz * interfaz) {
    interfaz->socket_with_kernel = socket;
}

// Devuelve el socket con el kernel:
int get_socket_kernel(t_interfaz * interfaz) {
    return interfaz->socket_with_kernel;
}

// Setea el nombre de la interfaz:
void set_interfaz_name(char* name, t_interfaz * interfaz) {
    interfaz->nombre = name;
}

// Devuelve el nombre de la interfaz:
char *get_interfaz_name(t_interfaz * interfaz) {
    return interfaz->nombre;
}

int get_tiempo_unidad(t_interfaz* interfaz){
    return interfaz->config->TIEMPO_UNIDAD_TRABAJO;
}



