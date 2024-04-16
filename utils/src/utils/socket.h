#ifndef UTILS_H_
#define UTILS_H_

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


extern t_log* logger;

int crear_conexion(t_log *logger, const char *server_name, char *ip, char *puerto);
void liberar_conexion(int socket_cliente);
int iniciar_servidor(t_log *logger, const char *name, char *ip, char *puerto);
int esperar_cliente(t_log *logger, const char *name, int socket_servidor);
void atender_conexion(t_log* logger, char* server_name, int cliente_socket);

#endif /* UTILS_H_ */
