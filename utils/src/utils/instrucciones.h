#ifndef INSTRUCCIONES_H
#define INSTRUCCIONES_H

#include <commons/collections/list.h>
#include <utils/protocolo.h>

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
    IO_TRUNCATE,
    IO_FS_TRUNCATE,
    IO_FD_WRITE,
    IO_FS_READ
} t_tipo_instruccion;

typedef struct {
    t_tipo_instruccion tipo;
    int cantParametros;
    t_list* parametros;
} t_instruccion;

void send_instruccion(int socket_cliente, char *instruccion, t_list *parametros);
t_instruccion* recv_instruccion(int socket_cliente);
t_instruccion *inicializar_instruccion(void);
void solicitar_instruccion(int socket_server, int PID, int program_counter);
t_tipo_instruccion obtener_tipo_instruccion(char *instruccion);
t_tipo_instruccion obtener_tipo_instruccion(char *instruccion);