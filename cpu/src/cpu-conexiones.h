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

#include "cpu.h"
#include "cpu-estructuras.h"
#include "cpu-ciclo-instrucciones.h"



// Funciones para manejo de clientes y conexiones del modulo:
void crear_servidores_cpu(int* md_cpu_ds, int* md_cpu_it);
void* generar_conexion_a_memoria(void* arg);
int generar_servidor_cpu_dispatch();
int generar_servidor_cpu_interrupt();




#endif