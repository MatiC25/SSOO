#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <commons/collections/list.h>
#include <utils/protocolo.h>

typedef enum {
    SET,
    SET, 
    SUM, 
    SUB, 
    JNZ,
    IO_GEN_SLEEP
} t_tipo_instruccion;

typedef struct {
    t_tipo_instruccion tipo;
    int cantParametros;
    t_list* parametros;
} t_instruccion;