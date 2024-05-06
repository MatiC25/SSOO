#ifndef INIT_CPU_H
#define INIT_CPU_H

#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/socket.h>
#include <utils/logger.h>

#include "cpu-estructuras.h"


//semaforo
pthread_mutex_t mutex_cpu;
pthread_mutex_t mutex_logger;

// Funciones para manejo de clientes y conexiones del modulo:
int crear_servidores(int* md_cpu_ds, int* md_cpu_it);
int generar_conexion_a_memoria(void);
int generar_servidor_cpu_dispatch(void);
int generar_servidor_cpu_interrupt(void);




#endif