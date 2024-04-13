#ifndef INIT_MEMORIA_H
#define INIT_MEMORIA_H

#include <stdio.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/socket.h>
#include <utils/shared.h>
#include <pthread.h>

typedef struct{

    int puerto_escucha;
    int tam_memoria;
    int tam_pagina;
    char* path_instrucciones;
    int retardo_respuesta;

} t_config_memoria;

typedef struct 
{
    t_log* logger_memoria;
    char* server_name;
    int socket_cliente;
} t_procesar_cliente;

typedef struct {
    t_log* logger_memoria;
    char* server_name;
    int socket_server;
} t_procesar_server;

extern t_log* logger_memoria;

int cargar_configuraciones(t_config_memoria *t_config_memoria, t_log *logger_memoria);
int crear_servidores(t_log* logger_memoria, t_config_memoria* config_memoria, int* md_generico);
void atender_conexiones_memoria(void *args);
void iniciar_modulo(t_log* logger_memoria, t_config_memoria* t_config_memoria);
void server_escuchar(t_log* logger_memoria, const char* server_name, int socket_server);
void cerrar_programa(t_log *logger_memoria);


#endif // INIT_MEMORIA_H