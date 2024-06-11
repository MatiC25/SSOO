#ifndef MAIN_H
#define MAIN_H

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
#include "instruc_memoria.h"
#include "inicializar_memoria.h"

extern t_dictionary* lista_instrucciones_porPID;
extern char* path_proceso;
extern t_bitarray* bitmap;
extern void* espacio_de_usuario;

#endif // MAIN_H