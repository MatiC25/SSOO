#include "io-peticiones.h"

void interfaz_recibir_peticiones(t_interfaz * interfaz, t_config_io* config_io) {
    if(interfaz->tipo == GENERICA)
        ejecutar_operacion_generica(interfaz, config_io);
    else if(interfaz->tipo == STDIN)
        ejecuta_operacion_stdin(interfaz, config_io);
    else if(interfaz->tipo == STDOUT)
        ejecutar_operacion_stdout(interfaz, config_io);
    else
        log_error(logger, "Tipo de interfaz no soportado");
}

void ejecutar_operacion_generica(t_interfaz * interfaz, t_config_io* config_io) {
    while(1) {
        int tiempo_espera = recibir_entero(interfaz);
        int tiempo_unidad = get_tiempo_unidad(config_io);

        // Realizamos la operacion:
        sleep(tiempo_espera * tiempo_unidad);

        // Enviamos la respuesta:
        enviar_respuesta_a_kernel(1, interfaz);
    }
}

void ejecutar_operacion_stdin(t_interfaz *interfaz, t_config_io *config_io) {
    char *input = NULL;

    while(1) {
        input = readline("Ingrese un valor: ");

        if(input == NULL) {
            log_error(logger, "Error al leer el input");
            exit(EXIT_FAILURE);
        }

        // Realizamos la operacion:
        int direccion_fisica = recibir_entero(interfaz);
        int bytes_a_leer = recibir_entero(interfaz);

        // Tomamos los bystes a leer de input:
        char *bytes = malloc(bytes_a_leer);
        memcpy(bytes, input, bytes_a_leer);

        // Enviamos los bytes a memoria:
        send_bytes_a_grabar(interfaz, direccion_fisica, bytes, bytes_a_leer);
        send_respuesta_a_kernel(1, interfaz);

        free(input);
        free(bytes);
    }
}

void ejecutar_operacion_stdout(t_interfaz *interfaz, t_config_io *config_io) {
    while(1) {
        int direccion_fisica = recibir_entero(interfaz);
        int bytes_a_mostrar = recibir_entero(interfaz);
        
        // Instanciamos los bytes a mostrar:
        char *bytes = malloc(bytes_a_mostrar);

        // Solicitamos los bytes a memoria:
        send_bytes_a_leer(interfaz, direccion_fisica, bytes_a_mostrar);
        rcv_bytes_a_leer(interfaz, bytes_a_mostrar, bytes);

        // Mostramos los bytes:
        log_info(logger, "Bytes a mostrar: %s", bytes);

        // Enviamos la respuesta:
        enviar_respuesta_a_kernel(1, interfaz);

        free(bytes);
    }
}