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
#include <utils/logger.h>
#include<readline/readline.h>

typedef enum { //podemos juntar todos los procesos 
    MENSAJE,
    PAQUETE,
    //INTERRUPT,
    RECIBIR_PROCESO,
   // EJECUTAR_INSTRUCCIONES
   SOLICITAR_INSTRUCCION,
   HANDSHAKE,
   OPERACION_IO,
   CREAR_INTERFAZ
   
// ----------------
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
void* serializar_paquete(t_paquete* paquete, int* bytes);
void eliminar_paquete(t_paquete* paquete);
void* recibir_buffer(int* size, int socket_cliente);

// Funciones para realizar operaciones basicas:
void enviar_mensaje(char* mensaje, int socket_cliente);
void recibir_mensaje(int socket_cliente);
void retardo_pedido(int tiempo_de_espera);

// Funciones para deserializcion:
int recibir_operacion(int socket_cliente);

// Operaciones de CPU:
void send_contexto_ejecucion(op_code operacion, int socket_cliente, t_pcb* proceso);
void agregar_a_paquete_PCB(t_paquete* paquete, t_pcb* proceso);
void agregar_a_paquete_registros(t_paquete* paquete, t_registro_cpu* registros);
void agregar_a_paquete_lista_string(t_paquete* paquete, t_list* archivos_abiertos);
void agregar_a_paquete_string(t_paquete* paquete, char* cadena, int tamanio);
t_pcb* rcv_contexto_ejecucion(int socket_cliente);
t_list *recv_list(int socket_cliente);
void enviar_buffer(void* buffer, size_t tamanio, int socket);
void recibir_program_counter(int socket_cpu,int *pid,int *program_counter);
void recv_archi_pid(int socket_cliente, char **path, int* pid);
// Funciones de saludo inicial:
void generar_handshake(int socket, char *server_name, char *ip, char *puerto);

#endif //PROTOCOLO_H
