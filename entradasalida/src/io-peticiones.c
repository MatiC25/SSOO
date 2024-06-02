#include "io-peticiones.h"

void interfaz_recibir_peticiones() {
    if(interfaz->tipo == GENERICA)
        ejecutar_operacion_generica();
}

void ejecutar_operacion_generica() {
    while(1) {
        int tiempo_espera = recibir_tiempo();
        int tiempo_unidad = get_tiempo_unidad();

        // Realizamos la operacion:
        sleep(tiempo_espera * tiempo_unidad);

        // Enviamos la respuesta:
        enviar_respuesta_a_kernel(1);
    }
}

// Funciones enviar mensajes a kernel:

void enviar_respuesta_a_kernel(int respuesta) {
    int socket_kernel = get_socket_kernel();
    send(socket_kernel, &respuesta, sizeof(int), 0);

    return;
}

// Funciones recibir mensajes de kernel:

int recibir_tiempo() {
    int tiempo_espera;
    int socket_kernel = get_socket_kernel();

    recv(socket_kernel, &tiempo_espera, sizeof(int), 0);

    return tiempo_espera;
}

// Funciones de metodos de interfaz: