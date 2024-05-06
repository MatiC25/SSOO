#ifndef CPU_CONFIG_H
#define CPU_CONFIG_H

#include <commons/config.h>
#include <commons/log.h>
#include <utils/shared.h>
#include <utils/logger.h>

#include "cpu-estructuras.h"

// Funcion para inicializar la configuracion de CPU:
t_config_cpu* inicializar_config(char *path_config_cpu);

// Funcion para cargar configuraciones de CPU:
void cargar_configuraciones(char* path_config_cpu);
