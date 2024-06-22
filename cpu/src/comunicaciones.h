#ifndef COMUNICACIONES_H
#define COMUNICACIONES_H

#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/socket.h>
#include <utils/logger.h>
#include <utils/instrucciones.h>
#include <utils/protocolo.h>
#include "cpu.h"
#include "cpu-estructuras.h"


int recv_pagina(int socket);
t_pcb_cpu* rcv_contexto_ejecucion_cpu(int socket_cliente);
void enviar_pcb_a_kernel(t_paquete* paquete_a_kernel);
void mostrar_pcb(t_pcb_cpu* pcb);
int recv_pagina(int socket);
void send_agrandar_memoria (int pid , int tamanio);
int recv_agrandar_memoria();
void enviar_pcb_a_kernel(t_paquete* paquete_a_kernel);
void enviar_a_leer_memoria(int pid,int direccionFIsica, int tamanio);
void send_escribi_memoria(int pid, int direccionFIsica, int tamanio, int valor);
int recv_escribir_memoria();
void solicitar_tablas_a_memoria(int numero_pagina);
t_tabla_de_paginas_cpu* recv_tablas();
void send_escribi_memoria_string(int pid,int direccionFIsica, int tamanio,char* valor);
char* recv_escribir_memoria_string(int tamanio);
int comunicaciones_con_memoria_escritura(t_mmu_cpu* mmu, int valor);
int comunicaciones_con_memoria_lectura(t_mmu_cpu* mmu);
void solicitar_a_kernel_std(char* interfaz ,int tamanio, t_list* direcciones_fisicas,t_paquete* solicitar_std);
void* recv_leer_memoria(int tamanio);


#endif