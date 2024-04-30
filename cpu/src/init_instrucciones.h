#ifndef INIT_INSTRUCCIONES_H
#define INIT_INSTRUCCIONES_H

#include <stdio.h>
#include <stdbool.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/socket.h>
#include <utils/shared.h>
#include <pthread.h>
#include <utils/estructuras_compartidas.h>
#include <utils/logger.h>
#include <pthread.h>
#include <utils/protocolo.h>

t_registro_cpu* t_registro;
t_pcb* pcb_cpu;

typedef enum
{
    LRU,
    FIFO
} t_algoritmo;




//semaforo
pthread_mutex_t mutex_cpu;
pthread_mutex_t mutex_logger;

void despach(t_instruccion* t_instrucciones);
void funcion_SET_cpu(char* registro, char* valor);
void funcion_SUM_cpu(t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroOrigen);
void funcion_SUB_cpu(t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroOrigen);
void funcion_JNZ_cpu(t_registro_cpu* t_registro,t_instruccion* t_instrucciones);
//void funcion_IO_GEN_SLEEP_cpu(/*INSTRUCCION*/,/*UNIDAD DE TRABAJO*/);
void function_MOV_IN_cpu(t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroDato);
void function_MOV_OUT_cpu(int tamanio);
void function_COPY_STRING_cpu(int tamanio);
void function_WAIT_cpu(int recurso);
void function_SINGAL_cpu(int recurso);
void function_IO_STDIN_READ_cpu(/*INTERFAZ*/, t_registro_cpu* t_registroDireccion,t_registro_cpu* t_registroTamanio);
void function_IO_STDOUT_WRITE_cpu(/*INTERFAZ*/,t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroTamanio);
void function_IO_FS_CREATE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo);
void function_IO_FS_DELETE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo);
void function_IO_FS_TRUNCATE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo);
void function_IO_FD_WRITE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo,t_registro_cpu* t_registroDireccion,t_registro_cpu* registroTamanio,t_registro_cpu registroPuntero);
void function_IO_FS_READ_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo,t_registro_cpu* t_registroDireccion,t_registro_cpu* registroTamanio,t_registro_cpu registroPuntero);



void fetchInstrucciones(t_registro_cpu *registro_cpu);


t_instruccion *leerProximaInstruccion(t_registro_cpu *registro, t_list *listaInstrucciones);


#endif