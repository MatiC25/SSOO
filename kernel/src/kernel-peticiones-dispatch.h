#ifndef KERNEL_PETICIONES_DISPATCH_H
#define KERNEL_PETICIONES_DISPATCH_H

#include <utils/estructuras_compartidas.h> 
#include <commons/collections/dictionary.h>
#include <utils/protocolo.h>
#include <errno.h>
#include <utils/instrucciones.h>
#include "kernel-config.h"
#include "planificacion.h"


void mover_a_bloqueado_por_wait(t_pcb* pcb, char* recurso);
int obtener_indice_recurso(char* recurso);
bool recurso_existe(char* recurso);
void hilo_motivo_de_desalojo();
void* escuchar_peticiones_dispatch();
const char* transformar_motivos_a_exit(t_tipo_instruccion *motivo_inicial);
void peticion_fin_quantum();
void peticion_exit(const char* tipo_de_exit);
void rcv_nombre_recurso(char **recurso, int socket);
int calcular_total_instancias() ;
void peticion_wait();
void peticion_signal();
void peticion_IO();
t_pcb* recibir_contexto_y_recurso();

#endif // KERNEL_PETICIONES_DISPATCH_H