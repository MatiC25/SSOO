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
#include "cpu-estructuras.h"

typedef enum
{
    LRU,
    FIFO

} t_algoritmo;

void iniciar_ciclo_de_ejecucion(int socket_server);
void ejecutar_ciclo_instrucciones(int socket_cliente, int socket_server);
void fecth(int socket_server);
void ejecutar_instruccion(int socket_cliente);
void ejecutar_set(char* registro, char valor);
void ejecutar_sum(char* registro_origen_char, char* registro_desitino_char);
void* obtener_registro (char *registro);
void ejecutar_sub(char* registro_origen_char, char* registro_desitino_char);
void ejecutar_JNZ(char* registro, char valor);
void enviar_pcb_a_kernel(t_paquete* paquete_a_kernel);
//void ejecutar_IO_GEN_SLEEP();


#endif