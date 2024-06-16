#include "kernel-utils-interfaces.h"

t_dictionary *dictionary_interfaces; // Diccionario de interfaces

// Funciones de manejo de interfaz desde el lado del kernel:

void set_name_interface(interface_io *interface, char *name) {
    interface->name = name;
}

int get_socket_interface(interface_io *interface) {
    return interface->socket_interface;
}

void set_socket_interface(interface_io *interface, int socket) {
    interface->socket_interface = socket;
}

void add_interface_to_dict(interface_io *interface, char *key) { 
    dictionary_put(dictionary_interfaces, key, interface);
}

interface_io *get_interface_from_dict(char *key) {
    return dictionary_get(dictionary_interfaces, key); 
}

int consulta_existencia_interfaz(interface_io *interface) {
    return interface != NULL;
}

void set_tipo_interfaz(interface_io *interface, tipo_interfaz tipo) {
    interface->tipo = tipo;
}

// Funciones para pedir operaciones a la interfaz:

int acepta_operacion_interfaz(interface_io *interface, tipo_operacion operacion) {
    tipo_operacion operaciones[] = {
        IO_GEN_SLEEP_INT,
        IO_STDIN_READ_INT,
        IO_STDOUT_WRITE_INT,
        IO_FS_CREATE_INT,
        IO_FS_DELETE_INT,
        IO_FS_TRUNCATE_INT,
        IO_FS_WRITE_INT,
        IO_FS_READ_INT
    };

    tipo_interfaz tipo = interface->tipo;
    
    if(tipo == GENERICA)
        return operacion == operaciones[0];
    else if(tipo == STDIN)
        return operacion == operaciones[1];
    else if(tipo == STDOUT)
        return operacion == operaciones[2];
    else if(tipo == DIALFS)
        return operacion == operaciones[3] || operacion == operaciones[4] || operacion == operaciones[5] || operacion == operaciones[6] || operacion == operaciones[7];
    else
        return 0; // Operacion invalida
}