#ifndef INIT_KERNEL_H
#define INIT_KERNEL_H

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
  int puerto_escucha;
  char *ip_memoria;
  int puerto_memoria;
  char *ip_cpu;
  int puerto_cpu_ds;
  int puerto_cpu_it;
  char *algoritmo_planificacion;
  int quantum;
  char **recursos;
  int *inst_recursos;
  int grado_multip;
} t_config_k;

typedef struct {
    t_log* logger_kernel;
    char* server_name;
    int socket_server;
} t_procesar_server;


extern t_log* logger_kernel;

int generar_conexiones(t_log *logger_kernel, t_config_k *config_kernel, int *md_memoria, int *md_cpu_dt, int *md_cpu_it);
int cargar_configuraciones(t_config_k *config_kernel, t_log *logger_kernel);
void cerrar_programa(t_log *logger_kernel);
void server_escuchar(void* args);
void iterator(char* value);
void iniciar_modulo(t_log* logger_kernel, t_config_k * config_kernel);
int crear_servidores(t_log* logger_kernel, t_config_k* config_kernel, int* md_EntradaySalida);

#endif // INIT_KERNEL_H