#include "io.h"

void interfaz_conectar(t_interfaz *interfaz) {
    t_paquete *paquete = crear_paquete(CREAR_INTERFAZ);

    agregar_a_paquete(paquete, &interfaz->tipo, sizeof(tipo_interfaz));
    agregar_a_paquete_string(paquete, interfaz->nombre);
    enviar_paquete(paquete, interfaz->config->socket_kernel);
}

void interfaz_recibir_peticiones() {
    if(interfaz -> tipo == GENERICA)
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

// Funciones recibir mensajes de kernel:

int recibir_tiempo() {
    int tiempo_espera;
    int socket_kernel = get_socket_kernel();

    recv(socket_kernel, &tiempo_espera, sizeof(int), 0);

    return tiempo_espera;
}


// Funciones enviar mensajes a kernel:

void enviar_respuesta_a_kernel(int respuesta) {
    int socket_kernel = get_socket_kernel();
    send(socket_kernel, &respuesta, sizeof(int), 0);

    return;
}