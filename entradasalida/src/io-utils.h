#ifndef IO_UTILS_H
#define IO_UTILS_H

// Incluye las estructuras necesarias:
#include "io-config.h"
#include "io-estructuras.h"

// Setea el socket con la memoria:
void set_socket_memory(int socket, t_interfaz *self);

// Devuelve el socket con la memoria:
int get_socket_memory(t_interfaz * interfaz);

// Setea el socket con el kernel:
void set_socket_kernel(int socket, t_interfaz *self) ;

// Devuelve el socket con el kernel:
void set_socket_kernel(int socket, t_interfaz *self);

// Setea el nombre de la interfaz:
int get_socket_kernel(t_interfaz *self);

// Devuelve el nombre de la interfaz:
void set_interfaz_name(char* name, t_interfaz *self) ;

char* get_interfaz_name(t_interfaz *self);

//Devuelve el tiempo de una interfaz
int get_tiempo_unidad(t_config_io* config_io);

#endif // IO_UTILS_H