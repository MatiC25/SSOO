#ifndef KERNEL_UTILS_INTERFACES_H
#define KERNEL_UTILS_INTERFACES_H

// Incluyendo bibliotecas necesarias:
#include <commons/collections/dictionary.h>
#include <utils/estructuras_compartidas.h>

// Incluyendo bibliotecas necesarias internas:
#include "kernel-estructuras.h"

extern t_dictionary *interfaces;

// Funciones de manejo de interfaz desde el lado del kernel
void set_name_interface(interface_io *interface, char *name);
int get_socket_interface(interface_io *interface);
void set_socket_interface(interface_io *interface, int socket);
void add_interface_to_dict(interface_io *interfaces, char *key);
interface_io *get_interface_from_dict(char *key);
int consulta_existencia_interfaz(interface_io *interface);

// Funciones para pedir operaciones a la interfaz
int consulta_interfaz_para_aceptacion_de_operacion(interface_io *interface);
int acepta_operacion_interfaz(interface_io *interface, tipo_operacion operacion);

#endif // KERNEL_UTILS_INTERFACES_H