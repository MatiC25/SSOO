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
#include <utils/estructuras_compartidas.h>

typedef struct 
{
    char* ip_memoria;
    int puerto_memoria;
    int puerto_escucha_dispatch;
    int puerto_escucha_interrupt;
    int cant_entradas;
    char* algoritmo;
} t_config_cpu;

// Funcion para cargar configuraciones de CPU:
int cargar_configuraciones(t_config_cpu *config_cpu, t_log *logger);

// Funciones para manejo de clientes y conexiones del modulo:
int crear_servidores(t_log* logger, t_config_cpu* config_cpu, int* md_cpu_ds, int* md_cpu_it);
int generar_conexiones(t_log *logger, t_config_cpu *config_cpu, int *md_memoria);

// Funciones de operaciones basicas del modulo:
void iniciar_modulo(t_log* logger_cpu, t_config_cpu* config_cpu);
void cerrar_programa(t_log* logger, t_config_cpu* config_cpu, int md_memoria);


#endif