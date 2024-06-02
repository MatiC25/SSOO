#ifndef IO_UTILS_H
#define IO_UTILS_H
#include "io.h"
// Setea el socket con la memoria:
void set_socket_memory(int socket);

// Devuelve el socket con la memoria:
int get_socket_memory();

// Setea el socket con el kernel:
void set_socket_kernel(int socket);

// Devuelve el socket con el kernel:
int get_socket_kernel();

// Setea el nombre de la interfaz:
void set_interfaz_name(char* name);

// Devuelve el nombre de la interfaz:
char* get_interfaz_name();

#endif // IO_UTILS_H