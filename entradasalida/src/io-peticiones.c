#include "io.h"

void interfaz_conectar(t_interfaz *interfaz) {
    t_paquete *paquete = crear_paquete(CREAR_INTERFAZ);

    agregar_a_paquete(paquete, &interfaz->tipo, sizeof(tipo_interfaz));
    agregar_a_paquete_string(paquete, interfaz->nombre);
    enviar_paquete(paquete, interfaz->config->socket_kernel);
}

void interfaz_recibir_peticiones() {
    while(1) {
        int operacion_a_realizar = recibir_operacion_de_kernel();
        validar_operacion(operacion_a_realizar);
    }
}

void validar_operacion() {
    tipo_operacion operacion = operacion_a_realizar();
    if(acepta_operacion_interfaz(operacion))
        enviar_respuesta_a_kernel(operacion, 1);
    else 
        ejecutar_operacion(operacion);
}

tipo_operacion operacion_a_realizar() {
    tipo_operacion operacion;
    recv(socket_kernel, &operacion, sizeof(tipo_operacion), 0);
    return operacion;
}

int acept_operacion_interfaz(tipo_operacion operacion) {
    int size_operaciones_validas = list_size(interfaz -> operaciones_validas);
    for(int i = 0; i < size_operaciones_validas; i++)
        if(operacion == list_get(interfaz -> operaciones_validas, i))
            return 0;
    
    return 1;
}

void ejecutar_operacion() {
    // Falta implementar
    enviar_respuesta_a_kernel(operacion, 0); // Esto señal que la operacion fue terminada
}