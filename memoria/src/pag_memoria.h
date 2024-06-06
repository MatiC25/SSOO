#ifndef PAG_MEMORIA_H
#define PAG_MEMORIA_H

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

//Estructuras para paginacion simple

typedef struct 
{
    int nro_pagina;
    int marco;
    int bit_validez;
    // bool presencia;
    // bool modificado;
} t_tabla_de_paginas;

#endif // PAG_MEMORIA_H