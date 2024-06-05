#ifndef CPU_CICLO_INSTRUCCIONES_H
#define CPU_CICLO_INSTRUCCIONES_H

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
#include "cpu.h"
#include "cpu-estructuras.h"

typedef enum{
    LRU,
    FIFO
} t_algoritmo;

int recv_escribir_memoria();
void send_escribi_memoria(int direccionFIsica, int tamanio,int valor);
void seguir_ciclo(int socket_cliente, int socket_server);
void enviar_a_leer_memoria(int direccionFIsica, int tamanio);
int recv_leer_memoria();
int espacio_de_registro(char* registro);
t_tabla_de_paginas_cpu* recv_tablas();
void solicitar_tablas_a_memoria(int numero_pagina);
void actualizar_lru_por_tlb(t_list* tlb, int numero_pagina);
void actualizar_lru_por_fifo(t_list* tlb, int numero_pagina);
t_tabla_de_paginas_cpu* buscarEnTLB(t_list* tablasCpu, int num_pagina);
t_mmu_cpu* traducirDireccion(int direccionLogica);
void solicitar_tablas_a_memoria(int numero_pagina);
void tengoAlgunaInterrupcion();
t_pcb_cpu* rcv_contexto_ejecucion_cpu(int socket_cliente);
void iniciar_ciclo_de_ejecucion(int socket_server ,int socket_cliente);
void ejecutar_ciclo_instrucciones(int socket_cliente, int socket_server);
void fecth(int socket_server);
void ejecutar_instruccion(int socket_cliente);
void ejecutar_set(char* registro, char* valor);
void ejecutar_sum(char* registro_origen_char, char* registro_desitino_char);
void* obtener_registro (char *registro);
void operar_con_registros(void* registro_destino, void* registro_origen, char* registro, char* operacion, int valor);
void ejecutar_sub(char* registro_origen_char, char* registro_desitino_char);
void ejecutar_JNZ(char* registro, char* valor);
void enviar_pcb_a_kernel(t_paquete* paquete_a_kernel);
void ejecutar_IO_GEN_SLEEP(char* interfazAUsar, char* tiempoDeTrabajo);
void ejecutar_MOV_IN(char* RegistroDatos, char* RegistroDireccion);
void ejecutar_MOV_OUT(char* RegistroDatos, char* RegistroDireccion);

#endif