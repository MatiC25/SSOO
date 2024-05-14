#ifndef PLANIFICACION_H
#define PLANIFICACION_H

#include <utils/logger.h>
#include <utils/estructuras_compartidas.h>
#include "init_kernel.h"
#include <semaphore.h>
#include <pthread.h>

pthread_mutex_t mutex_limite;


t_pcb* creacion_proceso(char** archivo_de_proceso, int quantum);
void agregar_a_cola_estado_new(t_pcb* proceso);
void mover_procesos_a_ready(int grado_multip);
int generar_pid_unico();

void inicializacion_semaforos();
void elegir_algoritmo();
void planificacion_fifo();
void planificacion_RR();
void planificacion_VRR();
void enviar_proceso_a_cpu(t_pcb* pcbproceso);


void liberar_pcb(t_pcb* estructura);
#endif