#define KERNEL_ESTRUCTURAS_H
#ifndef KERNEL_ESTRUCTURAS_H

#include <commons/list.h>
#include <commons/queue.h>

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
    int SOCKET_KERNEL;
    int connected; // Sirve para verificar si una I/O esta conectada
    t_queue *process_blocked; // Vamos a tener una cola de pid´s
} interface_io;

#endif