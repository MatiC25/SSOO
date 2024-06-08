#ifndef KERNEL_INTERFACES_H
#define KERNEL_INTERFACES_H


#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <utils/estructuras_compartidas.h>
#include <utils/protocolo.h>


#include "kernel-estructuras.h"

typedef struct {
    char *name;
    int SOCKET;
    tipo_interfaz tipo;
    t_queue *process_blocked;
    sem_t semaforo_used;
    sem_t size_blocked;
    t_list *operaciones_validas;
} interface_io;

// Variables globales:
extern t_dictionary *interfaces;
extern t_dictionary *args_consumers;
extern sem_t semaforo_interfaces;

// Funciones de manejo de interfaz desde el lado del kernel:
void handle_new_interface(void* arg);
void manage_interface(void *socket_cliente);

// Funciones de consumidores:
void create_consumer_thread(char *interface_name);
void consumers_pcbs_blockeds(void *args);

// Funciones manipuladoras de interfaz:
interface_io *initialize_interface();
void create_interface(int socket);

// Funciones de operaciones basicas de interfaz:
t_list *cargar_configuraciones_operaciones(tipo_interfaz tipo);
void agregar_operaciones(t_list *lista_operaciones, tipo_operacion operaciones[], tipo_interfaz tipo);

// Metodos de interfaz:
void set_name_interface(interface_io *interface, char *name);
int get_socket_interface(interface_io *interface);
void set_socket_interface(interface_io *interface, int socket);
void add_interface_to_dict(interface_io *interfaces, char *key);
interface_io *get_interface_from_dict(char *key);
void set_valid_operations(interface_io *interface, tipo_interfaz tipo);

// Funciones para pedir operaciones a la interfaz:
int consulta_existencia_interfaz(interface_io *interface);
int consulta_interfaz_para_aceptacion_de_operacion(interface_io *interface);
int acepta_operacion_interfaz(interface_io *interface, tipo_operacion operacion);

// Funciones para recibir mensajes de la interfaz:
tipo_operacion operacion_a_realizar(int socket);
tipo_interfaz recibir_tipo_interfaz(int socket);
void recibir_interfaz(char *interface_name, tipo_interfaz *tipo, int socket);

#endif // KERNEL_INTERFACES_H