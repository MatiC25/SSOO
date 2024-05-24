#ifndef KERNEL_INTERFACES_H
#define KERNEL_INTERFACES_H

#include "kernel-config.h"
#include "kernel.h"

// Funciones para inicializar:
interface_io *initialize_interface();

// Funciones para setear una interfaz y agregarla al diccionario:
void add_interface_to_dict(interface_io *interface, char *key);
interface_io *get_interface_from_dict(char *key);

// Funciones para manejar las interfaces:
void handle_new_interface(void *socket);
void manage_interface(void *socket_cliente);

// Funciones para crear hilos de consumidor para las interfaces:
void create_consumer_thread(char *interface_name);
void producer_pcbs_blockeds(void *args);

#endif // KERNEL_INTERFACES_H