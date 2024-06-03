#ifndef IO_H
#define IO_H

// Incluye las bibliotecas necesarias:

#include "io-conexiones.h"
#include "io-config.h"
#include "io-estructuras.h"
#include "io-init.h"
#include "io-conexiones.h"
#include "io-peticiones.h"
#include <utils/protocolo.h>



void interfaz_conectar(t_interfaz * interfaz);

//extern t_config_io* config_io;

#endif // IO_H