#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <commons/collections/list.h>
#include <utils/protocolo.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
//Generales    
    WAIT,
    SIGNAL,
    EXIT,

// CPU
    SET,
    MOVE_IN,
    MOV_OUT,
    SUM,
    SUB,
    JNZ,
    RESIZE,
    COPY_STRING,
    IO_GEN_SLEEP,
    IO_STDIN_READ,
    IO_STDOUT_WRITE,
    IO_FS_CREATE,
    IO_FS_DELETE,
    IO_FS_TRUNCATE,
    IO_FS_WRITE,
    IO_FS_READ
} t_tipo_instruccion;

// typedef struct {
//     t_tipo_instruccion tipo;
//     int cantParametros;
//     t_list* parametros;
// } t_instruccion;

typedef struct {  
    char* opcode;
    char* parametro1;
    char* parametro2;
    char* parametro3;
    char* parametro4;
    char* parametro5;
    int long_opcode;
    int long_par1;
    int long_par2;
    int long_par3;
    int long_par4;
    int long_par5;
} t_instruccion;

int recv_pagina(int socket_cliente);
void send_instruccion(int socket_cliente, char *instruccion, t_list *parametros);
t_instruccion* recv_instruccion(int socket_cliente);
void solicitar_instruccion(int socket_server, int PID, int program_counter);
t_tipo_instruccion obtener_tipo_instruccion(char *instruccion);
t_tipo_instruccion obtener_tipo_instruccion(char *instruccion);

#endif // INSTRUCCIONES_H