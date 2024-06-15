#ifndef INSTRUC_MEMORIA_H
#define INSTRUC_MEMORIA_H

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
#include <utils/instrucciones.h>
#include "estructuras_compartidas_memoria.h"

void leer_archivoPseudo(char *archivo_path);
char* crear_path_instrucciones(char* archivo_path);
void enviar_instruccion_a_cpu(int socket_cliente);

#endif // INSTRUC_MEMORIA_H