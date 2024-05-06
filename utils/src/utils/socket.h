#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <assert.h>
#include <utils/protocolo.h>
#include <utils/estructuras_compartidas.h>
#include <utils/logger.h>



// Funciones para crear de conexiones/servidores:
int crear_conexion(const char *server_name, char *ip, char *puerto);
int iniciar_servidor( const char *name, char *ip, char *puerto);

// Funciones para atender peticiones:
void atender_conexion(char* server_name, int cliente_socket);
int esperar_cliente(const char *name, int socket_servidor);
void server_escuchar_sin_hilos(void* args);
void server_escuchar_con_hilos( char* server_name, int socket_server);

// Funciones especificas de atender peticiones de los servidores:
void atender_conexiones_memoria(void *args);

// Funciones para liberar conexiones:
void liberar_conexion(int socket_cliente);
void iterator(char* value);

#endif /* SOCKET_H_ */