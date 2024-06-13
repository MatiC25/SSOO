#ifndef PETICIONES_H
#define PETICIONES_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/socket.h>
#include <utils/shared.h>
#include <utils/estructuras_compartidas.h>
#include <utils/protocolo.h>
#include <pthread.h>
#include <utils/logger.h>
#include "estructuras_compartidas_memoria.h"
#include "instruc_memoria.h"

void* escuchar_peticiones(void* args);
void handshake_desde_memoria(int socket_cliente);
void crear_proceso(int socket_cliente);
void terminar_proceso(int socket_cliente);
void obtener_marco(int socket_cliente);
void resize_proceso(int socket_cliente);
void acceso_lectura(int socket_cliente);
void acceso_escritura(int socket_cliente);
int obtener_marco_libre(t_bitarray* bitmap);
void liberar_marco(int marco);


#endif //PETICIONES_H