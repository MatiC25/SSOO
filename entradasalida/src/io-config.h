#ifndef IO_CONFIG_H
#define IO_CONFIG_H

// Incluye las bibliotecas necesarias:

//#include "io-estructuras.h"
#include <utils/estructuras_compartidas.h>
#include <utils/logger.h>
#include <utils/shared.h>
#include <commons/config.h>

// Incluye las estructuras necesarias:
#include "io-config.h"
#include "io-estructuras.h"
#include "io-utils.h"


// Funciones para inicializar:
t_config_io* inicializar_config_io();

// Funciones para configurar:
void configurar_valores_kernel(t_config_io* config_io, t_config* config);
void configurar_valores_memoria(t_config_io* config_io, t_config* config);
void configurar_tiempo_unidad(t_config_io* config_io, t_config* config);
void configurar_valores_dialfs(t_config_io* config_io, t_config* config);

#endif // IO_CONFIG_H