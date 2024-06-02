#ifndef PLANIFICACION_H
#define PLANIFICACION_H

#include <utils/logger.h>
#include <utils/estructuras_compartidas.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "kernel-init.h"

extern pthread_mutex_t mutex_estado_block;
extern pthread_mutex_t mutex_estado_ready;
extern pthread_mutex_t mutex_estado_new;
extern pthread_mutex_t mutex_estado_exec;
extern pthread_mutex_t mutex_cola_priori_vrr;
extern sem_t limite_grado_multiprogramacion;
extern sem_t habilitar_corto_plazo;
extern sem_t hay_en_estado_ready;
extern sem_t hay_en_estado_new;
extern sem_t hay_en_estado_new;

void creacion_proceso();
void agregar_a_cola_estado_new(t_pcb* proceso);
void agregar_a_cola_ready();
int generar_pid_unico();
void inicializacion_semaforos();
void elegir_algoritmo_corto_plazo(char* algoritmo);
void hilo_planificador_cortoplazo_fifo();
void* planificador_cortoplazo_fifo(void* arg);
void hilo_planificador_cortoplazo_RoundRobin();
void* planificador_corto_plazo_RoundRobin(void);
void enviar_proceso_a_cpu(t_pcb* pcbproceso);
void liberar_pcb(t_pcb* estructura);
void mover_procesos_de_ready_a_bloqueado(t_pcb* proceso);
void mover_procesos_de_bloqueado_a_ready(t_pcb* proceso);

// void creacion_proceso(t_pcb* proceso_nuevo);
// void agregar_a_cola_estado_new(t_pcb* proceso);
// void agregar_a_cola_ready();
// int generar_pid_unico();

// void inicializacion_semaforos();
// void elegir_algoritmo_corto_plazo(char* algoritmo);
// void hilo_planificador_cortoplazo_fifo();
// void* planificador_cortoplazo_fifo(void* arg);

// void hilo_planificador_cortoplazo_RoundRobin();
// void* planificador_corto_plazo_RoundRobin(voi   d);
// void planificacion_VRR();
// void enviar_proceso_a_cpu(t_pcb* pcbproceso);



// void liberar_pcb(t_pcb* estructura);
#endif