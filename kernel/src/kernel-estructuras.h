#ifndef KERNEL_ESTRUCTURAS_H
#define KERNEL_ESTRUCTURAS_H

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <utils/logger.h>
#include <kernel.h>

typedef struct {
    int PUERTO_ESCUCHA;
    char *IP_MEMORIA;
    int PUERTO_MEMORIA;
    char *IP_CPU;
    int PUERTO_CPU_DS;
    int PUERTO_CPU_IT;
    char *ALGORITMO_PLANIFICACION;
    int QUANTUM;
    char **RECURSOS;
    int *INST_RECURSOS;
    int GRADO_MULTIP;
    int SOCKET_DISPATCH;
    int SOCKET_INTERRUPT;
    int SOCKET_MEMORIA;
} t_config_kernel;

typedef struct {
    int SOCKET;
    int connected; // Sirve para verificar si una I/O esta conectada
    t_queue *process_blocked; // Vamos a tener una cola de pid´s
} interface_io;

extern t_config_kernel* config_kernel;

#endif