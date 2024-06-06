#ifndef KERNEL_PETICIONES_DISPATCH_H
#define KERNEL_PETICIONES_DISPATCH_H

#include <utils/estructuras_compartidas.h> 
#include <commons/collections/dictionary.h>
#include <utils/protocolo.h>
#include "kernel-config.h"
#include "planificacion.h"
typedef struct {
    tipo_desalojo desalojo;
    void* (*funcion)(void*); // Firma de las funciones!
} COMMAND;

extern COMMAND diccionario_peticiones[];

// Funciones que ejecutan las peticiones:
void* peticion_IO(t_pcb *pcb);

#endif // KERNEL_PETICIONES_DISPATCH_H