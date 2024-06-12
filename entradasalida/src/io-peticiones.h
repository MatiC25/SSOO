#ifndef IO_PETICIONES_H
#define IO_PETICIONES_H

#include <utils/estructuras_compartidas.h>
#include <utils/logger.h>
#include "io.h"
// Incluye las estructuras necesarias:
#include "io-config.h"
#include "io-estructuras.h"

#include "io-utils.h"

void interfaz_recibir_peticiones(t_interfaz * interfaz, t_config_io* config_io);
void ejecutar_operacion_generica(t_interfaz * interfaz, t_config_io* config_io);
void enviar_respuesta_a_kernel(int respuesta, t_interfaz * interfaz);
int recibir_tiempo(t_interfaz * interfaz);

#endif // IO_PETICIONES_H