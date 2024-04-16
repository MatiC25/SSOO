#ifndef PROTOCOLO_H
#define PROTOCOLO_H


#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include<assert.h>
#include <commons/config.h>
#include <commons/log.h> //se puede sacar
#include <commons/string.h>
#include <stddef.h>



typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	int size;
	void *stream;
} t_buffer;


typedef struct
{
	op_code codigo_operacion;
	t_buffer *buffer;
} t_paquete;

extern t_log* logger;

void iterator(char* value);
void recibir_mensaje(int socket_cliente);
void agregar_a_paquete(t_paquete *paquete, void *valor, int tamanio);
void enviar_paquete(t_paquete *paquete, int socket_cliente);
void *serializar_paquete(t_paquete *paquete, int bytes);
void eliminar_paquete(t_paquete *paquete);
void *recibir_buffer(int *, int);
void enviar_mensaje(char *mensaje ,int socket_cliente);
int recibir_operacion(int);
t_paquete *crear_paquete(void);
t_list *recibir_paquete(int);


#endif //PROTOCOLO_H