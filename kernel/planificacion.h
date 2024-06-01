#ifndef PLANIFICACION_H
#define PLANIFICACION_H

#include <utils/logger.h>
#include <utils/estructuras_compartidas.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include "kernel-init.h"

pthread_mutex_t mutex_estado_block;
pthread_mutex_t mutex_estado_ready;
pthread_mutex_t mutex_estado_new;
pthread_mutex_t mutex_estado_exit;
pthread_mutex_t mutex_estado_exec;
sem_t limite_grado_multiprogramacion;
sem_t habilitar_corto_plazo;
sem_t hay_en_estado_ready;
sem_t hay_en_estado_new;

void inicializacion_semaforos();
int generar_pid_unico();
void informar_a_memoria(char** archivo_de_proceso, int pid);
void creacion_proceso();
void agregar_a_cola_estado_new(t_pcb* proceso);
void agregar_a_cola_ready();
void mover_procesos_de_ready_a_bloqueado(t_pcb* proceso);
void mover_procesos_de_bloqueado_a_ready(t_pcb* proceso);
t_pcb* obtener_siguiente_a_ready();
void finalizar_proceso(t_pcb* proceso, const char* motivo);
void liberar_proceso(t_pcb* proceso);
void elegir_algoritmo_corto_plazo();
void hilo_planificador_largoplazo();
void* planificacion_largoplazo();
void hilo_planificador_cortoplazo_fifo();
void hilo_planificador_cortoplazo_RoundRobin();
void* planificador_cortoplazo_fifo(void* arg);
void* planificador_corto_plazo_RoundRobin(void* arg);
void* quantum_handler(void* arg);
t_pcb* rcv_contexto_ejecucion(int socket_cliente);
void enviar_proceso_a_cpu(t_pcb* pcbproceso);
void prevent_from_memory_leaks();



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