#ifndef KERNEL_PROTOCOLO_H
#define KERNEL_PROTOCOLO_H

// Incluyendo bibliotecas externas:
#include <commons/collections/list.h>
#include <utils/protocolo.h>
#include <utils/estructuras_compartidas.h>

// Incluyendo bibliotecas internas:
#include "kernel-estructuras.h"
#include "kernel-utils-interfaces.h"

// Funciones para enviar mensajes a la interfaz:
void send_message_to_interface(interface_io *interface, t_list *args, int *response, int socket);
void send_message_to_generic_interface(int socket, t_list *args, int *response);
void send_message_to_std_interface(int socket, t_list *args, int *response);
void send_message_to_dialfs_interface(int socket, t_list *args, int *response);
void send_message_to_dialfs_create_o_delete(int socket, t_list *args, int *response, int *operacion_a_realizar);
void send_message_to_dialfs_read_o_write(int socket, t_list *args, int *response, int *operacion_a_realizar);
void send_message_to_dialfs_truncate(int socket, t_list *args, int *response);

// Funciones para recibir mensajes de interfaces:
void rcv_interfaz(char **interface_name, tipo_interfaz *tipo, int socket);
void rcv_nombre_interfaz_dispatch(char **interface_name, int socket);
t_list *recv_interfaz_y_argumentos(int socket);

// Funciones auxiliares:
int parsear_int(void *buffer, int *desplazamiento);
char *parsear_string(void *buffer, int *desplazamiento);
t_list *obtener_argumentos(void *buffer, int *desplazamiento, int operacion_a_realizar);
void obtener_argumentos_generica(t_list *argumentos, void *buffer, int *desplazamiento);
void obtener_argumentos_std(t_list *argumentos, void *buffer, int *desplazamiento);
void obtener_argumentos_dialfs_create_o_delete(t_list *argumentos, void *buffer, int *desplazamiento, int operacion_a_realizar);
void obtener_argumentos_dialfs_read_o_write(t_list *argumentos, void *buffer, int *desplazamiento, int operacion_a_realizar);
void obtener_argumentos_dialfs_truncate(t_list *argumentos, void *buffer, int *desplazamiento, int operacion_a_realizar);
void rcv_nombre_interfaz(char **interface_name, void *buffer, int *desplazamiento, int *size);
        
#endif // KERNEL_PROTOCOLO_H