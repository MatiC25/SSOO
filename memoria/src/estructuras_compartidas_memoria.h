#ifndef ESTRUCTURAS_COMPARTIDAS_MEMORIA_H
#define ESTRUCTURAS_COMPARTIDAS_MEMORIA_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <utils/socket.h>
#include <utils/shared.h>
#include <utils/estructuras_compartidas.h>
#include <utils/protocolo.h>
#include <utils/logger.h>
#include <pthread.h>

typedef struct 
{
    int puerto_escucha;
    int tam_memoria;
    int tam_pagina;
    char* path_instrucciones;
    int retardo_respuesta;

} t_config_memoria;

typedef struct {
    int nro_pagina;
    int marco;
    int bit_validez;
} t_tabla_de_paginas;

extern t_dictionary* lista_instrucciones_porPID;
extern t_dictionary * diccionario_paginas_porPID;
extern t_tabla_de_paginas* tabla_de_paginas;
extern t_bitarray* bitmap;
extern t_config_memoria* config_memoria;
extern void* espacio_de_usuario;
extern char* path_proceso;

#endif // ESTRUCTURAS_COMPARTIDAD_MEMORIA_H