#ifndef INIT_KERNEL_H
#define INIT_KERNEL_H

#include <stdio.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>

typedef struct
{
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

bool generar_conexiones(t_log *logger, t_config_k *config_kernel, int *md_memoria, int *md_cpu_dt, int *md_cpu_it);
bool cargar_configuraciones(t_config_k *config_kernel, t_log *logger);
void cerrar_programa(t_log *logger);

#endif // INIT_KERNEL_H