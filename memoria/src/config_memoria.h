#ifndef CONFIG_MEMORIA_H
#define CONFIG_MEMORIA_H

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/socket.h>
#include <utils/shared.h>
#include <utils/estructuras_compartidas.h>
#include <utils/protocolo.h>
#include <utils/logger.h>
#include <utils/instrucciones.h>
#include "estructuras_compartidas_memoria.h"

int cargar_configuraciones_memoria(t_config_memoria* config_memoria);
t_config_memoria* inicializar_config_memoria(void);
void config_destroy_version_memoria(t_config_memoria* config_memoria);

#endif // CONFIG_MEMORIA_H