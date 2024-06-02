#ifndef KERNEL_ESTRUCTURAS_H
#define KERNEL_ESTRUCTURAS_H

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <utils/logger.h>
#include <kernel.h>
#include <semaphore.h>
#include <pthread.h>
#include <commons/collections/dictionary.h>

typedef struct {
    int PUERTO_ESCUCHA;
    char *IP_MEMORIA;
    int PUERTO_MEMORIA;
    char *IP_CPU;
    char *ALGORITMO_PLANIFICACION;
    int QUANTUM;
    char **RECURSOS;
    int *INST_RECURSOS;
    int GRADO_MULTIP;
    int SOCKET_DISPATCH;
    int SOCKET_INTERRUPT;
    int SOCKET_MEMORIA;
    int PUERTO_CPU_DS;
    int PUERTO_CPU_IT;
    int PUERTO_KERNEL;
    char* IP_KERNEL;
} t_config_kernel;


extern t_config_kernel* config_kernel;

extern t_list* cola_new;
extern t_list* cola_ready; 
extern t_list* cola_block;
extern t_list* cola_prima_VRR;

#endif