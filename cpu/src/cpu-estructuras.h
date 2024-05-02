#ifndef CPU_ESTRUCTURAS_H
#define CPU_ESTRUCTURAS_H

#include <utils/estructuras_compartidas.h>

typedef struct {
    char *IP_MEMORIA;
    int PUERTO_MEMORIA;
    int PUERTO_ESCUCHA_DISPATCH;
    int PUERTO_ESCUCHA_INTERRUPT;
    int CANTIDAD_ENTRADAS_TLB; 
    char *ALGORITMO_TLB;
    int SOCKET_MEMORIA;
    int SOCKET_DISPATCH;
} t_config_cpu;

typedef struct {
    int PID;
    int program_counter;
    // t_instruccion *instruccion;
    t_registro_cpu *registro;
} t_pcb_cpu;

extern t_config_cpu *config_cpu;

#endif