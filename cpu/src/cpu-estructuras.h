#ifndef CPU_ESTRUCTURAS_H
#define CPU_ESTRUCTURAS_H

#include <utils/estructuras_compartidas.h>
#include <utils/instrucciones.h>

typedef struct {
    char *IP_MEMORIA;
    int PUERTO_MEMORIA;
    int PUERTO_ESCUCHA_DISPATCH;
    int PUERTO_ESCUCHA_INTERRUPT;
    int CANTIDAD_ENTRADAS_TLB; 
    char *ALGORITMO_TLB;
    int SOCKET_MEMORIA;
    int SOCKET_DISPATCH;
    int SOCKET_KERNEL;
    int TAMANIO_PAGINA;
} t_config_cpu;

typedef struct { 
    int pid;
    int program_counter; 
    t_registro_cpu* registros;
    t_list* tabla_cpu;
}t_pcb_cpu;

typedef struct {
    int pid;
    int nropagina;
    int marco;
    int contador; //Para el LRU 
} t_tabla_de_paginas_cpu;

typedef struct {
    t_list* num_pagina; 
    int ofset;
    t_list* direccionFIsica;
    t_list* tamanio;
}t_mmu_cpu;

extern t_pcb_cpu* pcb;
//extern t_mmu_cpu* mmu;



#endif