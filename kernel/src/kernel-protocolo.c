#include "kernel-protocolo.h"

// Funciones para enviar mensajes a interfaces:

void send_message_to_interface(interface_io *interface, t_list *args, int *response, int socket) {
    switch(interface->tipo) {
        case GENERICA:
            send_message_to_generic_interface(socket, args, response);
            break;
        case STDIN:
        case STDOUT:
            // send_message_to_std_interface(get_socket_interface(interface), args, response);
            break;
        case DIALFS:
            // send_message_to_dialfs_interface(get_socket_interface(interface), args, response);
            break;
        default:
            // Manejo de error o caso por defecto
            break;
    }
}

void send_message_to_generic_interface(int socket, t_list *args, int *response) {
    int *tiempo_sleep_ptr = list_get(args, 0);

    if (tiempo_sleep_ptr == NULL) {
        fprintf(stderr, "Error: list_get retornó NULL\n");
        return;
    }

    int tiempo_sleep = *tiempo_sleep_ptr;

    // Enviar el valor de tiempo_sleep
    if (send(socket, &tiempo_sleep, sizeof(int), 0) == -1) {
        perror("Error al enviar el mensaje");
        return;
    }

    // Recibir la respuesta
    if (recv(socket, response, sizeof(int), 0) == -1) {
        perror("Error al recibir la respuesta");
        return;
    }
}

void send_message_to_std_interface(int socket, t_list *args, int *response) {
    int *direccion_fisica_ptr = list_get(args, 0);
    int *bytes_a_leer_ptr = list_get(args, 1);

    if (direccion_fisica_ptr == NULL || bytes_a_leer_ptr == NULL) {
        fprintf(stderr, "Error: list_get retornó NULL\n");
        return;
    }

    int direccion_fisica = *direccion_fisica_ptr;
    int bytes_a_leer = *bytes_a_leer_ptr;

    // Enviar el valor de direccion_fisica
    if (send(socket, &direccion_fisica, sizeof(int), 0) == -1) {
        perror("Error al enviar direccion_fisica");
        return;
    }

    // Enviar el valor de bytes_a_leer
    if (send(socket, &bytes_a_leer, sizeof(int), 0) == -1) {
        perror("Error al enviar bytes_a_leer");
        return;
    }

    // Recibir la respuesta
    if (recv(socket, response, sizeof(int), 0) == -1) {
        perror("Error al recibir la respuesta");
        return;
    }
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

void rcv_nombre_interfaz_dispatch(char **interface_name, int socket) {
    int tamanio;
    int desplazamiento = 0;
    int size = 0;
    void *buffer = recibir_buffer(&size, socket);
    
    rcv_nombre_interfaz(interface_name, buffer, &desplazamiento, &tamanio);
}

// t_list *rcv_argumentos_para_io(tipo_interfaz tipo_de_interfaz, int socket) {
//     t_list *args = list_create();

//     switch(tipo_de_interfaz) {
//         case GENERICA:
//             rcv_argumentos_para_io_generica(args, socket);
//             break;
//         case STDIN:
//         case STDOUT:
//             rcv_argumentos_para_io_std(args, socket);
//             break;
//         default:
//             // Manejo de error o caso por defecto
//             break;
//     }

//     return args;
// }

// void rcv_argumentos_para_io_generica(t_list *args, void *buffer, int *desplazamiento, int socket) {
//     int tiempo_sleep;
//     memcpy(&tiempo_sleep, buffer + *desplazamiento, sizeof(int));
//     *desplazamiento += sizeof(int);

//     int *tiempo_sleep_ptr = malloc(sizeof(int));
//     if (tiempo_sleep_ptr == NULL) {
//         // Manejo de error si malloc falla
//         fprintf(stderr, "Error: malloc falló\n");
//         return;
//     }

//     *tiempo_sleep_ptr = tiempo_sleep;
//     list_add(args, tiempo_sleep_ptr);
// }

// void rcv_argumentos_para_io_std(t_list *args, int socket) {
//     int size;
//     int desplazamiento = 0;
//     int direccion_fisica;
//     int bytes_a_leer;
//     void *buffer = recibir_buffer(&size, socket);

//     if (buffer == NULL) {
//         // Manejo de error si recibir_buffer retorna NULL
//         fprintf(stderr, "Error: recibir_buffer retornó NULL\n");
//         return;
//     }

//     memcpy(&direccion_fisica, buffer + desplazamiento, sizeof(int));
//     desplazamiento += sizeof(int);

//     memcpy(&bytes_a_leer, buffer + desplazamiento, sizeof(int));
//     desplazamiento += sizeof(int);

//     // Asignar memoria para los enteros antes de añadirlos a la lista
//     int *direccion_fisica_ptr = malloc(sizeof(int));
//     int *bytes_a_leer_ptr = malloc(sizeof(int));

//     if (direccion_fisica_ptr == NULL || bytes_a_leer_ptr == NULL) {
//         // Manejo de error si malloc falla
//         fprintf(stderr, "Error: malloc falló\n");
//         free(direccion_fisica_ptr);
//         free(bytes_a_leer_ptr);
//         free(buffer); // Liberar el buffer recibido
//         return;
//     }

//     *direccion_fisica_ptr = direccion_fisica;
//     *bytes_a_leer_ptr = bytes_a_leer;

//     list_add(args, direccion_fisica_ptr);
//     list_add(args, bytes_a_leer_ptr);

//     // Liberar el buffer recibido
//     free(buffer);
// }

t_list * recv_interfaz_y_argumentos(int socket) {
    int tipo;
    t_list *args = list_create();
    t_list *interfaz_y_argumentos = list_create();

    recv(socket, &tipo, sizeof(int), 0);

    log_info(logger, "Tipo de interfaz: %i", tipo);

    int size;
    int desplazamiento = 0;
    int argumento;
    int tamanio;
    void *buffer = recibir_buffer(&size, socket);

    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer");
        return;
    }

    // Deserializar el tipo
    // Crear una copia de 'tipo' para agregar a la lista
    int *tipo_copia = malloc(sizeof(int));
    if (tipo_copia == NULL) {
        log_error(logger, "Error al asignar memoria para tipo");
        free(buffer);
        return;
    }
    *tipo_copia = tipo;

    list_add(interfaz_y_argumentos, tipo_copia); // 0

    // log_info(logger, "Tipo de interfaz: %d", tipo);

    // Deserializar el argumento
    memcpy(&argumento, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    // log_info(logger, "Argumento: %d", argumento);

    // Crear una copia de 'argumento' para agregar a la lista
    int *argumento_copia = malloc(sizeof(int));
    *argumento_copia = argumento;
    list_add(args, argumento_copia); 
    list_add(interfaz_y_argumentos, args); // 1
    // Deserializar el tamaño del nombre de la interfaz
    memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    log_info(logger, "Tamaño del nombre de la interfaz: %d", tamanio);

    // Verificar y asignar memoria para el nombre de la interfaz
    char *interface_name = malloc(tamanio);
    if (interface_name == NULL) {
        log_error(logger, "Error al asignar memoria para el nombre de la interfaz");
        free(tipo_copia);
        free(buffer);
        return;
    }
    memcpy(interface_name, buffer + desplazamiento, tamanio);
    desplazamiento += tamanio;

    // log_info(logger, "Nombre de la interfaz: %s", interface_name);
    list_add(interfaz_y_argumentos, interface_name); // 2
    // log_info(logger, "Argumento: %d", argumento);

    // Liberar el buffer después de usarlo
    free(buffer);

    //     memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
    //     void *argumento = malloc(tamanio);
    //     desplazamiento += sizeof(int);
    //     memcpy(argumento, buffer + desplazamiento, tamanio);
    //     desplazamiento += tamanio;

    //     list_add(args, argumento);
    // }

    return interfaz_y_argumentos;
}

// t_list *rcv_argumentos_para_io(tipo_interfaz tipo_de_interfaz, void *buffer, int *desplazamiento) {
//     t_list *args = list_create();

//     switch(tipo_de_interfaz) {
//         case GENERICA:
//             rcv_argumentos_para_io_generica(args, buffer, desplazamiento);
//             break;
//         case STDIN:
//         case STDOUT:
//             // rcv_argumentos_para_io_std(args, buffer, desplazamiento);
//             break;
//         default:
//             // Manejo de error o caso por defecto
//             break;
//     }

//     return args;
// }

void rcv_nombre_interfaz(char **interface_name, void *buffer, int *desplazamiento, int *size) { 
    // Primero, copiamos el tamaño del nombre de la interfaz desde el buffer al tamaño especificado
    memcpy(size, buffer + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    // Reservamos memoria para el nombre de la interfaz, asegurándonos de que no sea NULL
    *interface_name = malloc(*size);
    if (*interface_name == NULL) {
        log_error(logger, "Error al reservar memoria para el nombre de la interfaz");
        return;
    }

    // Copiamos el nombre de la interfaz desde el buffer a la memoria reservada
    memcpy(*interface_name, buffer + *desplazamiento, *size);
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