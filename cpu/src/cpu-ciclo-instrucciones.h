#ifndef CPU_CICLO_INSTRUCCIONES_H
#define CPU_CICLO_INSTRUCCIONES_H

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/socket.h>
#include <utils/logger.h>
#include <utils/instrucciones.h>
#include "cpu.h"

typedef enum
{
    LRU,
    FIFO

} t_algoritmo;


//semaforo
pthread_mutex_t mutex_cpu;
pthread_mutex_t mutex_logger


void iniciar_ciclo_de_ejecucion(int socket_server);
void ejecutar_ciclo_instrucciones(int socket_cliente, int socket_server);
t_pcb* recibir_pcb_a_kernel(int socket_cliente);
void fecth(int socket_server);
void ejecutar_instruccion(int socket_cliente);
void ejecutar_set(char* registro, int valor);
void ejecutar_sum(char* registro_origen_char, char* registro_desitino_char);
t_registro_cpu* obtener_registro (char *registro);
void ejecutar_sub(char* registro_origen_char, char* registro_desitino_char);
void ejecutar_JNZ(char* registro, int valor);
//void ejecutar_IO_GEN_SLEEP();


#endif