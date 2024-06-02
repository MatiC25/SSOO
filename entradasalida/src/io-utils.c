#include "io-utils.h"

//  Setea el socket con la memoria:
void set_socket_memory(int socket) {
    interfaz->socket_with_kernel = socket;
}

// Devuelve el socket con la memoria:
int get_socket_memory() {
    return interfaz->socket_with_kernel;
}

// Setea el socket con el kernel:
void set_socket_kernel(int socket) {
    interfaz->socket_with_memoria = socket;
}

// Devuelve el socket con el kernel:
int get_socket_kernel() {
    return interfaz->socket_with_memoria;
}

// Setea el nombre de la interfaz:
void set_interfaz_name(char* name) {
    interfaz->nombre = name;
}

// Devuelve el nombre de la interfaz:
char* get_interfaz_name() {
    return interfaz->nombre;
}

int get_tiempo_unidad(){
    return config_io->TIEMPO_UNIDAD_UNIDAD;
}



