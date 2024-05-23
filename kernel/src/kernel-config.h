#define KERNEL_CONFIG_H
#ifndef KERNEL_CONFIG_H

#include <commons/config.h>
#include <commons/log.h>
#include <utils/shared.h>
#include <utils/logger.h>

#include "kernel-estructuras.h"

t_config_kernel* inicializar_config(void);

void cargar_configuraciones(t_config_kernel* config_kernel);