#ifndef KERNEL_ESTRUCTURAS_H
#define KERNEL_ESTRUCTURAS_H

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <utils/logger.h>
#include <utils/estructuras_compartidas.h>
#include <semaphore.h>
#include <pthread.h>

#define MAX_RECURSOS 3
typedef struct {
    char* PUERTO_ESCUCHA;
    char *IP_MEMORIA;
    char* PUERTO_MEMORIA;
    char *IP_CPU;
    char *ALGORITMO_PLANIFICACION;
    int QUANTUM;
    char **RECURSOS; // Lista de recursos
    int *INST_RECURSOS; // Lista de cantidad de recursos
    int GRADO_MULTIP;
    int SOCKET_DISPATCH;
    int SOCKET_INTERRUPT;
    int SOCKET_MEMORIA;
    char* PUERTO_CPU_DS;
    char* PUERTO_CPU_IT;
    int PUERTO_KERNEL;
    char* IP_KERNEL;
} t_config_kernel;

typedef struct {
    int PID;
    char* recurso;
} t_recursos_pedidos;

typedef struct {
    char *name;
    int socket_interface;
    tipo_interfaz tipo;
    t_queue *process_blocked;
    t_queue *args_process;
    sem_t semaforo_used;
    sem_t size_blocked;
} interface_io;


extern t_config_kernel* config_kernel;

extern t_list* cola_new;
extern t_list* cola_ready; 
extern t_list* cola_prima_VRR;
extern t_list* cola_block;
extern t_queue* colas_resource_block[MAX_RECURSOS];

extern t_recursos_pedidos* vector_recursos_pedidos;
extern int tam_vector_recursos_pedidos;

#endif