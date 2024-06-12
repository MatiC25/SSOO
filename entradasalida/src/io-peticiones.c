#include "io-peticiones.h"

void interfaz_recibir_peticiones(t_interfaz * interfaz, t_config_io* config_io) {
    if(interfaz->tipo == GENERICA)
        ejecutar_operacion_generica(interfaz, config_io);
}

void ejecutar_operacion_generica(t_interfaz * interfaz, t_config_io* config_io) {
    while(1) {
        int tiempo_espera = recibir_tiempo(interfaz);
        int tiempo_unidad = get_tiempo_unidad(config_io);

        // Realizamos la operacion:
        sleep(tiempo_espera * tiempo_unidad);

        // Enviamos la respuesta:
        enviar_respuesta_a_kernel(1, interfaz);
    }
}

// Funciones enviar mensajes a kernel:

void enviar_respuesta_a_kernel(int respuesta, t_interfaz * interfaz) {
    int socket_kernel = get_socket_kernel(interfaz);
    send(socket_kernel, &respuesta, sizeof(int), 0);

    return;
}

// Funciones recibir mensajes de kernel:

int recibir_tiempo(t_interfaz * interfaz) {
    int tiempo_espera;
    int socket_kernel = get_socket_kernel(interfaz);

    recv(socket_kernel, &tiempo_espera, sizeof(int), 0);

    return tiempo_espera;
}

// Funciones de metodos de interfaz: