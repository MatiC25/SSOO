#include "io-protocolo.h"

// Funciones enviar mensajes a kernel:

void send_respuesta_a_kernel(int respuesta, t_interfaz * interfaz) {
    int socket_kernel = get_socket_kernel(interfaz);
    send(socket_kernel, &respuesta, sizeof(int), 0);

    return;
}

void send_interfaz_a_kernel(t_interfaz * interfaz) {
    t_paquete *paquete = crear_paquete(CREAR_INTERFAZ);
    int socket_cliente = get_socket_kernel(interfaz);

    agregar_a_paquete(paquete, &interfaz->tipo, sizeof(tipo_interfaz));
    agregar_a_paquete_string(paquete, interfaz->nombre, strlen(interfaz->nombre) + 1);
    enviar_paquete(paquete, socket_cliente);
}

// Funciones recibir mensajes de kernel:

int recibir_entero(t_interfaz * interfaz) {
    int dato;
    int socket_kernel = get_socket_kernel(interfaz);

    recv(socket_kernel, &dato, sizeof(int), 0);

    return dato;
}

// Funciones enviar mensajes a memoria:

void send_bytes_a_grabar(t_interfaz * interfaz, int direccion_fisica, char *bytes, int bytes_a_leer) {
    int socket_memoria = get_socket_memory(interfaz);
    t_paquete *paquete = crear_paquete(ESCRIBIR_MEMORIA);

    agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
    agregar_a_paquete(paquete, &bytes_a_leer, sizeof(int));
    agregar_a_paquete_string(paquete, bytes, bytes_a_leer);

    enviar_paquete(paquete, socket_memoria);
}

void send_mensaje_a_memoria(t_interfaz * interfaz, char *mensaje) {
    int socket_memoria = get_socket_memory(interfaz);
    t_paquete *paquete = crear_paquete(MENSAJE);

    agregar_a_paquete_string(paquete, mensaje, strlen(mensaje) + 1);

    enviar_paquete(paquete, socket_memoria);
    eliminar_paquete(paquete);
}

void send_bytes_a_leer(t_interfaz *interfaz, int direccion_fisica, int bytes_a_mostrar) {
    int socket_memoria = get_socket_memory(interfaz);
    t_paquete *paquete = crear_paquete(LEER_MEMORIA);

    agregar_a_paquete(paquete, &direccion_fisica, sizeof(int));
    agregar_a_paquete(paquete, &bytes_a_mostrar, sizeof(int));

    enviar_paquete(paquete, socket_memoria);
}


// Funciones recibir mensajes de memoria:

void rcv_bytes_a_leer(t_interfaz *interfaz, int bytes_a_mostrar, char **bytes) {
    int size;
    int socket_memoria = get_socket_memory(interfaz);

    // Recibimos el buffer:
    void *buffer = recibir_buffer(&size, socket_memoria);

    // copiamos el buffer a bytes:
    memcpy(*bytes, buffer, bytes_a_mostrar);
}