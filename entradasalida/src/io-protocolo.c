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

t_list *recibir_arguementos(t_interfaz * interfaz, tipo_operacion tipo) {
    int size;
    int desplazamiento = 0;
    int socket_kernel = get_socket_kernel(interfaz);

    // Recibimos el buffer:
    void *buffer = recibir_buffer(&size, socket_kernel);
    
    // Parseamos el buffer para obtener los argumentos:
    char *file = parsear_string(buffer, &desplazamiento);
    list_add(argumentos, file); // Agregamos el archivo a la lista de argumentos


    if (tipo == IO_FS_WRITE_INT || tipo == IO_FS_READ_INT) {
        // Parseamos el buffer para obtener la direccion:
        int direccion = parsear_int(buffer, &desplazamiento);
        int *direccion_puntero = malloc(sizeof(int));
        *direccion_puntero = direccion;
        list_add(argumentos, direccion_puntero); // Agregamos la direccion a la lista de argumentos

        // Parseamos el buffer para obtener la cantidad de bytes a leer/escribir:
        int bytes = parsear_int(buffer, &desplazamiento);
        int *bytes_puntero = malloc(sizeof(int));
        *bytes_puntero = bytes;
        list_add(argumentos, bytes_puntero); // Agregamos la cantidad de bytes a la lista de argumentos
    }

    if(tipo == IO_FS_WRITE_INT || tipo == IO_FS_READ_INT || tipo == IO_FS_TRUNCATE_INT) {
        // Parseamos el buffer para obtener desde donde vamos a leer/escribir:
        int offset = parsear_int(buffer, &desplazamiento);
        int *offset_puntero = malloc(sizeof(int));
        *offset_puntero = offset;
        list_add(argumentos, offset_puntero); // Agregamos el offset a la lista de argumentos
    }

    // Liberamos el buffer recibido:
    free(buffer);

    return argumentos;
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
    
    // Copiamos el buffer a bytes:
    memcpy(*bytes, buffer, bytes_a_mostrar);

    // Liberamos el buffer recibido:
    free(buffer);
}

void rcv_contenido_a_escribir(t_interfaz *interfaz, int bytes_a_escribir, unsigned char **contenido) {
    int size;
    int socket_memoria = get_socket_memory(interfaz);

    // Recibimos el buffer:
    void *buffer = recibir_buffer(&size, socket_memoria);
    
    // Copiamos el buffer a contenido:
    memcpy(*contenido, buffer, bytes_a_escribir);

    // Liberamos el buffer recibido:
    free(buffer);
}

// Funciones auxiliares:

char *parsear_string(void *buffer, int *desplazamiento) {   
    int tam;

    // Copiamos el tamaño del string:
    memcpy(&tam, buffer + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    // Copiamos el string:
    char *string = malloc(tam);
    memcpy(string, buffer + *desplazamiento, tam);
    *desplazamiento += tam;

    return string;
}

int parsear_int(void *buffer, int *desplazamiento) {
    int dato;

    // Copiamos el dato:
    memcpy(&dato, buffer + *desplazamiento, sizeof(int));
    *desplazamiento += sizeof(int);

    return dato;
}