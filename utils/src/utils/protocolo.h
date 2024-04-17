#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>
#include <commons/config.h>
#include <commons/log.h> 
#include <commons/string.h>
#include <stddef.h>
#include <utils/estructuras_compartidas.h>

typedef enum {
    MENSAJE,
    PAQUETE,
    INICIAR_PROCESO,
    EJECUTAR_PROCESO,
    INTERRUPT,
    RECIBIR_PROCESO,
    DEVOLVER_PROCESO,
} op_code;
typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

// uint8_t: 1 byte - 8 bits -> 0 a 255

// Funciones para manejo de serializacion: 
t_paquete* crear_paquete(op_code operacion);
void crear_buffer(t_paquete* paquete);
void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio);
void enviar_paquete(t_paquete* paquete, int socket_cliente);
void* serializar_paquete(t_paquete* paquete, int bytes);
void eliminar_paquete(t_paquete* paquete);
void* recibir_buffer(int* size, int socket_cliente);

// Funciones para realizar operaciones basicas:
void enviar_mensaje(char* mensaje, int socket_cliente);
void recibir_mensaje(t_log *logger, int socket_cliente);

// Funciones para deserializcion:
int recibir_operacion(int socket_cliente);

// Operaciones de CPU:
void send_contexto_ejecucion(op_code operacion, int socket_cliente, t_pcb* proceso);
void agregar_a_paquete_PCB(t_paquete* paquete, t_pcb* proceso);
void agregar_a_paquete_registros(t_paquete* paquete, t_registros_cpu* registros);
void agregar_a_paquete_archivos_abiertos(t_paquete* paquete, t_list* archivos_abiertos);
void agregar_a_paquete_string(t_paquete* paquete, char* cadena, int tamanio);
t_pcb* rcv_contexto_ejecucion(int socket_cliente);

#endif //PROTOCOLO_H