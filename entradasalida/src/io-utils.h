#ifndef IO_UTILS_H
#define IO_UTILS_H

#include "io-config.h"
#include "io-estructuras.h"

// Incluye las estructuras necesarias:

// Setea el socket con la memoria:
void set_socket_memory(int socket, t_interfaz * interfaz);

// Devuelve el socket con la memoria:
int get_socket_memory(t_interfaz * interfaz);

// Setea el socket con el kernel:
void set_socket_kernel(int socket, t_interfaz * interfaz) ;

// Devuelve el socket con el kernel:
void set_socket_kernel(int socket, t_interfaz * interfaz);

// Setea el nombre de la interfaz:
int get_socket_kernel(t_interfaz * interfaz);

// Devuelve el nombre de la interfaz:
void set_interfaz_name(char* name, t_interfaz * interfaz) ;

// Devuelve el nombre de la interfaz:
char *get_interfaz_name(t_interfaz * interfaz);

// Devuelve el tiempo de una interfaz:
int get_tiempo_unidad(t_interfaz * interfaz);

// Devuelve el path de una interfaz:
char *get_path_dial_fs(t_interfaz * interfaz);

// Devuelve el tamaño del bitmap:
size_t get_tamanio_bitmap(t_interfaz *interfaz);

// Devuelve la cantidad de bloques:
int get_block_count(t_interfaz *interfaz);

// Devuelve el tamaño de un bloque:
int get_block_size(t_interfaz *interfaz);

// Devuelve el total de bytes de una lista de direcciones:
int get_total_de_bytes(t_list *direcciones);

// Devuelve el nombre de una operación:
char *get_nombre_operacion(tipo_operacion operacion);

// Devuelve el tipo de interfaz:
tipo_interfaz get_tipo_interfaz_to_int(t_interfaz *interfaz);

// Devuelve el tamaño de un archivo:
int ordenar_direcciones_por_tamanio(void *direccion1, void *direccion2);

#endif // IO_UTILS_H