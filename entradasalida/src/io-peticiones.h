#ifndef IO_PETICIONES_H
#define IO_PETICIONES_H

#include <readline/readline.h>
#include <utils/estructuras_compartidas.h>
#include <utils/logger.h>
// Incluye las estructuras necesarias:

#include "io-config.h"
#include "io-estructuras.h"
#include "io-utils.h"


// Funciones para recibir peticiones:
void interfaz_recibir_peticiones(t_interfaz * interfaz, t_config_io* config_io);

// Funciones para ejecutar operaciones:
void ejecutar_operacion_stdin(t_interfaz *interfaz, t_config_io *config_io);
void ejecutar_operacion_stdout(t_interfaz *interfaz, t_config_io *config_io);


#endif // IO_PETICIONES_H