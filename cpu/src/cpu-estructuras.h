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
} t_config_cpu;



// typedef struct {
//     int tamano_pagina; 
//     int num_pagina; //Puede ser floor 
//     int desplazamiento;//puede ser floor
//     int 
// }t_mmu_cpu;

#endif