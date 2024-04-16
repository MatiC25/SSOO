#ifndef INIT_CPU_H
#define INIT_CPU_H

#include <stdio.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/socket.h>
#include <utils/shared.h>
#include <pthread.h>

typedef struct 
{
    char* ip_memoria;
    int puerto_memoria;
    int puerto_escucha_dispatch;
    int puerto_escucha_interrupt;
    int cant_entradas;
    char* algoritmo;
} t_config_cpu;


typedef struct {
    t_log* logger;
    char* server_name;
    int socket_server;
} t_procesar_server;


extern t_log* logger;


int crear_servidor(t_log *logger, const char *name, char *ip, char *puerto);
int generar_conexiones(t_log *logger, t_config_cpu *config_cpu, int *md_memoria);
int cargar_configuraciones(t_config_cpu *config_cpu, t_log *logger);
void cerrar_programa(t_log *logger);
void iterator(char* value);
void server_escuchar(void* args);
void iniciar_modulo(t_log* logger_cpu, t_config_cpu* config_cpu);


#endif