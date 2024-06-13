#ifndef KERNEL_ESTRUCTURAS_H
#define KERNEL_ESTRUCTURAS_H

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <utils/logger.h>
#include <kernel.h>
#include <semaphore.h>
#include <pthread.h>
#include <commons/collections/dictionary.h>
#define MAX_RECURSOS 3
typedef struct {
    int PUERTO_ESCUCHA;
    char *IP_MEMORIA;
    int PUERTO_MEMORIA;
    char *IP_CPU;
    char *ALGORITMO_PLANIFICACION;
    int QUANTUM;
    char **RECURSOS; // Lista de recursos
    int *INST_RECURSOS; // Lista de cantidad de recursos
    int GRADO_MULTIP;
    int SOCKET_DISPATCH;
    int SOCKET_INTERRUPT;
    int SOCKET_MEMORIA;
    int PUERTO_CPU_DS;
    int PUERTO_CPU_IT;
    int PUERTO_KERNEL;
    char* IP_KERNEL;
} t_config_kernel;

typedef struct {
    int PID;
    char* recurso;
} t_recursos_pedidos;


extern t_config_kernel* config_kernel;
extern t_list* cola_new;
extern t_list* cola_ready; 
extern t_list* cola_prima_VRR;

extern t_queue* cola_block[MAX_RECURSOS];

extern t_recursos_pedidos* vector_recursos_pedidos;
extern int tam_vector_recursos_pedidos;

#endif