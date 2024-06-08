#ifndef PETICIONES_H
#define PETICIONES_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/socket.h>
#include <utils/shared.h>
#include <utils/estructuras_compartidas.h>
#include <utils/protocolo.h>
#include <pthread.h>
#include <utils/logger.h>
#include "main.h"


void crear_proceso(socket_cliente);
void terminar_proceso(socket_cliente);
void obtener_marco(socket_cliente);
void modificar_tamaño_memoria(socket_cliente, config_memoria->retardo_respuesta);
void acceso_lectura(socket_cliente);
void acceso_escrituratura(socket_cliente);


#endif //PETICIONES_H