#ifndef KERNEL_PROTOCOLO_H
#define KERNEL_PROTOCOLO_H

#include <commons/collections/list.h>
#include <utils/protocolo.h>
#include <utils/estructuras_compartidas.h>

// Funciones para enviar mensajes a la interfaz:
void send_message_to_interface(interface_io *interface, t_list *args, int *response);
void send_message_to_generic_interface(int socket, t_list *args, int *response);
void send_message_to_std_interface(int socket, t_list *args, int *response);

// Funciones para recibir mensajes de la interfaz:
void rcv_interfaz(char **interface_name, tipo_interfaz *tipo, int socket);
void rcv_nombre_interfaz(char **interface_name, void *buffer, int *desplazamiento, int *size);
void rcv_argumentos_para_io_generica(t_list *args);
void rcv_argumentos_para_io_std(t_list *args);
t_list *rcv_argumentos_para_io(tipo_interfaz tipo_de_interfaz);
tipo_interfaz recibir_tipo_interfaz(int socket);
tipo_operacion operacion_a_realizar(int socket);

#endif // KERNEL_PROTOCOLO_H