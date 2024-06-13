#ifndef INICIALIZAR_MEMORIA_H
#define INICIALIZAR_MEMORIA_H

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
#include <utils/logger.h>
#include <utils/protocolo.h>
#include "peticiones_memoria.h"
#include "estructuras_compartidas_memoria.h"

// Funciones para manejo de clientes y conexiones del modulo:
int crear_servidores(t_config_memoria* config_memoria, int *md_generico);

// Funciones de operaciones basicas del modulo:
int iniciar_modulo(t_config_memoria* t_config_memoria);
void cerrar_programa(t_config_memoria *config_memoria, int socket_server);
void* escuchar_peticiones(void* args);

#endif // INICIALIZAR_MEMORIA_H