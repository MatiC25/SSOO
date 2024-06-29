#ifndef IO_PETICIONES_H
#define IO_PETICIONES_H

//#include "io-estructuras.h"
#include <readline/readline.h>
#include <utils/estructuras_compartidas.h>
#include <utils/logger.h>
#include <commons/bitarray.h>

// Incluye las estructuras necesarias:
#include "io-config.h"
#include "io-estructuras.h"
#include "io-utils.h"
#include "io-protocolo.h"
#include "io-operaciones-dial-fs.h"
#include "io-utils-dial-fs.h"

// Funciones para enviar peticiones:
void interfaz_recibir_peticiones(t_interfaz* interfaz);

// Funciones para ejecutar operaciones:
void ejecutar_operacion_generica(t_interfaz * interfaz);
void ejecutar_operacion_stdin(t_interfaz *interfaz);
void ejecutar_operacion_stdout(t_interfaz *interfaz);
void ejecutar_operaciones_dialFS(t_interfaz *interfaz);

#endif // IO_PETICIONES_H