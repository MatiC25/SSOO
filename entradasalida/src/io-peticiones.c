#include "io-peticiones.h"

void interfaz_recibir_peticiones(t_interfaz * interfaz) {
    if(interfaz->tipo == GENERICA)
        ejecutar_operacion_generica(interfaz);
    else if(interfaz->tipo == STDIN)
        ejecutar_operacion_stdin(interfaz);
    else if(interfaz->tipo == STDOUT)
        ejecutar_operacion_stdout(interfaz);
    else if(interfaz->tipo == DIALFS)
        ejecutar_operaciones_dialFS(interfaz);
    else
        log_error(logger, "Tipo de interfaz no soportado");
}

void ejecutar_operacion_generica(t_interfaz * interfaz) {
    while(1) {
        // Obtenemos los argumentos:
        t_list *argumentos = recibir_argumentos(interfaz, get_socket_kernel(interfaz));

        // Obtenemos los argumentos necesarios:
        int *pid_proceso = list_get(argumentos, 0);
        int *tipo_operacion = list_get(argumentos, 1);
        int *tiempo_espera = list_get(argumentos, 2);
        char *operacion = get_nombre_operacion(*tipo_operacion);
        int tiempo_unidad = get_tiempo_unidad(interfaz);

        // Logeamos la operación:
        log_info(logger, "PID: %d - Operacion: %s", *pid_proceso, operacion);
        log_info(logger, "Tiempo de espera de segundos: %d", *tiempo_espera * tiempo_unidad);

        // Esperamos el tiempo de espera:
        sleep(*tiempo_espera * tiempo_unidad);

        // Enviamos la respuesta al kernel:
        send_respuesta_a_kernel(1, interfaz);
    }
}

void ejecutar_operacion_stdin(t_interfaz *interfaz) {

    // Inicializamos las variables:
    char *input = NULL;

    while(1) {
        // Obtenemos los argumentos:
        t_list *argumentos = recibir_argumentos(interfaz, get_socket_kernel(interfaz));

        // Obtenemos los argumentos necesarios:
        int *pid_proceso = list_get(argumentos, 0);
        int *tipo_operacion = list_get(argumentos, 1);
        t_list *direcciones = list_get(argumentos, 2);
        char *operacion = get_nombre_operacion(*tipo_operacion);

        // Logeamos la operación:
        log_info(logger, "PID: %d - Operacion: %s", *pid_proceso, operacion);

    
        int bytes_a_escribir = get_total_de_bytes(direcciones);

        log_info(logger, "Bytes a escribir: %i", bytes_a_escribir);

        // Leemos el input:
        input = readline("Ingrese un valor: ");

        if(input == NULL) {
            log_error(logger, "Error al leer el input");
            exit(EXIT_FAILURE);
        }

        while(strlen(input) > bytes_a_escribir || strlen(input) < bytes_a_escribir) {
            log_error(logger, "El input ingresado es mayor al tamaño de bytes a leer");
            input = readline("Ingrese un valor: ");
        }

        int bytes_leidos = strlen(input);

        // Enviamos los bytes a escribir a memoria:
        send_bytes_a_leer(interfaz, *pid_proceso, direcciones, input, bytes_leidos);
        send_respuesta_a_kernel(1, interfaz);

        free(input);
    }
}

void ejecutar_operacion_stdout(t_interfaz *interfaz) {
    while(1) {
        // Obtenemos los argumentos:
        t_list *argumentos = recibir_argumentos(interfaz, get_socket_kernel(interfaz));

        // Obtenemos los argumentos necesarios:
        int *pid_proceso = list_get(argumentos, 0);
        int *tipo_operacion = list_get(argumentos, 1);
        t_list *direcciones = list_get(argumentos, 2);
        char *operacion = get_nombre_operacion(*tipo_operacion);
        
        // Logeamos la operación:
        log_info(logger, "PID: %d - Operacion: %s", *pid_proceso, operacion);

        // Solicitamos los bytes a memoria:
        char *contenido_a_mostrar = rcv_contenido_a_mostrar(interfaz, direcciones, *pid_proceso);

        // Mostramos el contenido:
        log_info(logger, "PID: %i - Contenido leido: %s", *pid_proceso, contenido_a_mostrar);

        // Enviamos la respuesta:
        send_respuesta_a_kernel(1, interfaz);

        free(contenido_a_mostrar);
    }
}

void ejecutar_operaciones_dialFS(t_interfaz *interfaz) {

    // Inicializamos las variables:
    int tiempo_respuesta_retardo = get_tiempo_unidad(interfaz);
    int socket_kernel = get_socket_kernel(interfaz);

    // Inicializamos archivos abiertos:
    t_list *archivos_abiertos = traer_archivos_abiertos(interfaz);

    // Inicializamos modo de apertura:
    char *modo_de_apertura = get_modo_de_apertura(archivos_abiertos);

    // Inicializamos bloques:
    FILE *bloques = iniciar_archivo_bloques(interfaz, modo_de_apertura);

    // Inicializamos bitmap:
    t_bitarray *bitmap = iniciar_bitmap(interfaz, modo_de_apertura);

    // Traemos archivos persistidos:

    if (!archivos_abiertos) // Si no hay archivos abiertos, inicializamos el bitmap
        inicializar_bloques_vacios(bitmap, interfaz); // Inicializamos los bloques vacios

    while(1) {
        tipo_operacion operacion = recibir_operacion(socket_kernel);
        t_list *argumentos = recibir_argumentos_para_dial(interfaz, operacion);

        switch(operacion) {
            case IO_FS_CREATE_INT:
                operacion_create_file(interfaz, bitmap, argumentos, archivos_abiertos);
                send_respuesta_a_kernel(1, interfaz);
                break;
            case IO_FS_READ_INT:
                operacion_read_file(interfaz, bloques, argumentos, archivos_abiertos);
                send_respuesta_a_kernel(1, interfaz);
                break;
            case IO_FS_DELETE_INT:
                operacion_delete_file(interfaz, bitmap, argumentos, archivos_abiertos);
                send_respuesta_a_kernel(1, interfaz);
                break;
            case IO_FS_WRITE_INT:
                operacion_write_file(interfaz, bloques, argumentos, archivos_abiertos);
                send_respuesta_a_kernel(1, interfaz);
                break;
            case IO_FS_TRUNCATE_INT:
                operacion_truncate_file(interfaz, bloques, bitmap, argumentos, archivos_abiertos);
                send_respuesta_a_kernel(1, interfaz);
                break;
            default:
                log_error(logger, "Operacion desconocida");
        }
    }
}