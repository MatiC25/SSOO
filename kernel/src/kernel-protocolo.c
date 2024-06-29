#include "kernel-protocolo.h"

// 1. Funciones para enviar mensajes a interfaces:

void send_message_to_interface(interface_io *interface, t_list *args, int *response, int socket) {
    switch(interface->tipo) {
        case GENERICA:
            send_message_to_generic_interface(socket, args, response);
            break;
        case STDIN:
        case STDOUT:
            send_message_to_std_interface(get_socket_interface(interface), args, response);
            break;
        case DIALFS:
            send_message_to_dialfs_interface(get_socket_interface(interface), args, response);
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
    if (send(socket, &tiempo_sleep, sizeof(int), 0) == -1) {
        perror("Error al enviar el mensaje");
        return;
    }

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

    if (send(socket, &direccion_fisica, sizeof(int), 0) == -1) {
        perror("Error al enviar direccion_fisica");
        return;
    }

    if (send(socket, &bytes_a_leer, sizeof(int), 0) == -1) {
        perror("Error al enviar bytes_a_leer");
        return;
    }

    if (recv(socket, response, sizeof(int), 0) == -1) {
        perror("Error al recibir la respuesta");
        return;
    }
}

void send_message_to_dialfs_interface(int socket, t_list *args, int *response) {
    int *operacion_a_realizar = list_get(args, 0);

    switch(*operacion_a_realizar) {
        case IO_FS_CREATE_INT:
        case IO_FS_DELETE_INT:
            send_message_to_dialfs_create_o_delete(socket, args, response, operacion_a_realizar);
            break;
        case IO_FS_READ_INT:
        case IO_FS_WRITE_INT:
            send_message_to_dialfs_read_o_write(socket, args, response, operacion_a_realizar);
            break;
        case IO_FS_TRUNCATE_INT:
            send_message_to_dialfs_truncate(socket, args, response);
            break;
        default:
            // Manejo de error o caso por defecto
            break;
    }
}

void send_message_to_dialfs_create_o_delete(int socket, t_list *args, int *response, int *operacion_a_realizar) {
    char *nombre_archivo = list_get(args, 1);

    t_paquete *paquete = crear_paquete(*operacion_a_realizar);
    agregar_a_paquete_string(paquete, nombre_archivo, strlen(nombre_archivo) + 1);
    enviar_paquete(paquete, socket);

    eliminar_paquete(paquete);
}

void send_message_to_dialfs_read_o_write(int socket, t_list *args, int *response, int *operacion_a_realizar) {
    char *nombre_archivo = list_get(args, 1);
    int *offset = list_get(args, 2);
    int *bytes_a_leer_o_escribir = list_get(args, 3);

    t_paquete *paquete = crear_paquete(*operacion_a_realizar);
    agregar_a_paquete_string(paquete, nombre_archivo, strlen(nombre_archivo) + 1);
    agregar_a_paquete(paquete, bytes_a_leer_o_escribir, sizeof(int));
    agregar_a_paquete(paquete, offset, sizeof(int));
    enviar_paquete(paquete, socket);

    eliminar_paquete(paquete);
}

void send_message_to_dialfs_truncate(int socket, t_list *args, int *response) {
    char *nombre_archivo = list_get(args, 1);
    int *tamanio = list_get(args, 2);

    t_paquete *paquete = crear_paquete(IO_FS_TRUNCATE_INT);
    agregar_a_paquete_string(paquete, nombre_archivo, strlen(nombre_archivo) + 1);
    agregar_a_paquete(paquete, tamanio, sizeof(int));
    enviar_paquete(paquete, socket);

    eliminar_paquete(paquete);
}

// 2. Funciones para recibir mensajes de interfaces:

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

t_list * recv_interfaz_y_argumentos(int socket) {
    int size;
    int desplazamiento = 0;
    t_list *interfaz_y_argumentos = list_create();

    int operacion_a_realizar = recibir_operacion(socket);
    int *operacion_a_realizar_ptr = malloc(sizeof(int));
    *operacion_a_realizar_ptr = operacion_a_realizar;
    list_add(interfaz_y_argumentos, operacion_a_realizar_ptr);

    void *buffer = recibir_buffer(&size, socket);

    char *nombre_interfaz = parsear_string(buffer, &desplazamiento);
    list_add(interfaz_y_argumentos, nombre_interfaz);

    t_list *argumentos = obtener_argumentos(buffer, &desplazamiento, operacion_a_realizar);
    list_add(interfaz_y_argumentos, argumentos);

    return interfaz_y_argumentos;
}

// 3. Funciones auxiliares:

int parsear_int(void *buffer, int *desplazamiento) {
    int dato;

    memcpy(&dato, buffer + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    return dato;
}

char *parsear_string(void *buffer, int *desplazamiento) {
    int tamanio;
    char *string;

    memcpy(&tamanio, buffer + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    string = malloc(tamanio);
    if (string == NULL) {
        log_error(logger, "Error al reservar memoria para el string");
        return NULL;
    }

    memcpy(string, buffer + *desplazamiento, tamanio);
    *desplazamiento += tamanio + 1;

    return string;
}

t_list *obtener_argumentos(void *buffer, int *desplazamiento, int operacion_a_realizar) {

    t_list *argumentos = list_create();

    switch(operacion_a_realizar) {
        case IO_GEN_SLEEP_INT:
            obtener_argumentos_generica(argumentos, buffer, desplazamiento);
            break;
        case IO_STDIN_READ_INT:
        case IO_STDOUT_WRITE_INT:
            obtener_argumentos_std(argumentos, buffer, desplazamiento);
            break;
        case IO_FS_CREATE_INT:
        case IO_FS_DELETE_INT:
            obtener_argumentos_dialfs_create_o_delete(argumentos, buffer, desplazamiento, operacion_a_realizar);
            break;
        case IO_FS_READ_INT:
        case IO_FS_WRITE_INT:
            obtener_argumentos_dialfs_read_o_write(argumentos, buffer, desplazamiento, operacion_a_realizar);
            break;
        case IO_FS_TRUNCATE_INT:
            obtener_argumentos_dialfs_truncate(argumentos, buffer, desplazamiento, operacion_a_realizar);
            break;
        default:
            // Manejo de error o caso por defecto
            break;
    }

    return argumentos;
}

void obtener_argumentos_generica(t_list *argumentos, void *buffer, int *desplazamiento) {
    int *tiempo_sleep = malloc(sizeof(int));
    *tiempo_sleep = parsear_int(buffer, desplazamiento);

    list_add(argumentos, tiempo_sleep);
}

void obtener_argumentos_std(t_list *argumentos, void *buffer, int *desplazamiento) {
    int *direccion_fisica = malloc(sizeof(int));
    int *bytes_a_leer = malloc(sizeof(int));

    *direccion_fisica = parsear_int(buffer, desplazamiento);
    *bytes_a_leer = parsear_int(buffer, desplazamiento);

    list_add(argumentos, direccion_fisica);
    list_add(argumentos, bytes_a_leer);
}

void obtener_argumentos_dialfs_create_o_delete(t_list *argumentos, void *buffer, int *desplazamiento, int operacion_a_realizar) {
    char *nombre_archivo = parsear_string(buffer, desplazamiento);

    int *operacion_a_realizar_ptr = malloc(sizeof(int));
    *operacion_a_realizar_ptr = operacion_a_realizar;

    list_add(argumentos, operacion_a_realizar_ptr);
    list_add(argumentos, nombre_archivo);
}

void obtener_argumentos_dialfs_read_o_write(t_list *argumentos, void *buffer, int *desplazamiento, int operacion_a_realizar) {
    char *nombre_archivo = parsear_string(buffer, desplazamiento);
    int *bytes_a_leer_o_escribir = malloc(sizeof(int));
    int *offset = malloc(sizeof(int));

    *bytes_a_leer_o_escribir = parsear_int(buffer, desplazamiento);
    *offset = parsear_int(buffer, desplazamiento);

    int *operacion_a_realizar_ptr = malloc(sizeof(int));
    *operacion_a_realizar_ptr = operacion_a_realizar;

    list_add(argumentos, operacion_a_realizar_ptr);
    list_add(argumentos, nombre_archivo);
    list_add(argumentos, bytes_a_leer_o_escribir);
    list_add(argumentos, offset);
}

void obtener_argumentos_dialfs_truncate(t_list *argumentos, void *buffer, int *desplazamiento, int operacion_a_realizar) {
    char *nombre_archivo = parsear_string(buffer, desplazamiento);
    int *tamanio = malloc(sizeof(int));

    *tamanio = parsear_int(buffer, desplazamiento);

    int *operacion_a_realizar_ptr = malloc(sizeof(int));
    *operacion_a_realizar_ptr = operacion_a_realizar;

    list_add(argumentos, operacion_a_realizar_ptr);
    list_add(argumentos, nombre_archivo);
    list_add(argumentos, tamanio);
}

void rcv_nombre_interfaz(char **interface_name, void *buffer, int *desplazamiento, int *size) { 

    memcpy(size, buffer + *desplazamiento, sizeof(int));

    *desplazamiento += sizeof(int);
    *interface_name = malloc(*size);

    if (*interface_name == NULL) {
        log_error(logger, "Error al reservar memoria para el nombre de la interfaz");
        return;
    }

    memcpy(*interface_name, buffer + *desplazamiento, *size);    
    *desplazamiento += *size;
}