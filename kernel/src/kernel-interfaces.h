#ifndef KERNEL_INTERFACES_H
#define KERNEL_INTERFACES_H

#include "kernel-config.h"

typedef struct {
    int connected;
    int SOCKET;
    char *name;
    t_queue* process_blocked;
    sem_t semaforo_blocked;
} interface_io;

// Funciones de manejo de interfaz desde el lado del kernel:
void handle_new_interface(void* arg); // Maneja la creación de una nueva interfaz
void manage_interface(void *socket_cliente); // Administra la interfaz

// Funciones de consumidores:
void create_consumer_thread(char *interface_name); // Crea un hilo consumidor
void consumers_pcbs_blockeds(void *args); // Maneja los PCBs bloqueados

// Funciones manipuladoras de interfaz:
interface_io *initialize_interface(); // Inicializa una interfaz
void create_interface(int socket); // Crea una interfaz

// Metodos de interfaz:
void set_name_interface(interface_io *interface, char *name); // Establece el nombre de la interfaz
void add_interface_to_dict(interface_io *interfaces, char *key); // Agrega la interfaz al diccionario
interface_io *get_interface_from_dict(char *key); // Obtiene la interfaz del diccionario

// Funciones para pedir operaciones a la interfaz:
int consulta_interfaz_para_aceptacion_de_operacion(interface_io *interface, tipo_operacion operacion); // Consulta a la interfaz si acepta la operación
int consulta_existencia_interfaz(interface_io *interface); // Consulta si la interfaz existe

#endif // KERNEL_INTERFACES_H