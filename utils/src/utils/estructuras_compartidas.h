#ifndef ESTRUCTURAS_SHARED_H_
#define ESTRUCTURAS_SHARED_H_

#include <commons/log.h>
#include <stdint.h>
#include <utils/logger.h>
#include <stdlib.h>

typedef enum {
    GENERICA,
    STDIN,
    STDOUT,
    DIALFS
} tipo_interfaz;

typedef struct {
    char* server_name;
    int socket_cliente;
} t_procesar_conexion;


typedef struct {
    char* server_name;
    int socket_servidor;
} t_procesar_server;


typedef struct {
    uint32_t PC;
    uint8_t AX, BX, CX, DX;
    uint32_t EAX, EBX, ECX, EDX;
    uint32_t SI; // Cambiado a puntero
    uint32_t DI; // Cambiado a puntero
} t_registro_cpu;

typedef enum {
    NEW,
    READY,
    EXEC,
    BLOCK,
    EXITT,
} status_cod;

typedef struct { // PCB de un proceso 
    int pid;
    int program_counter; 
    int64_t quantum; 
    t_registro_cpu* registros;
    int estado;
}t_pcb;

typedef enum {
    INSTRUCCION_IO,
    FIN_QUANTUM,
    WAIT,
    SIGNAL,
    FIN_DE_PROCESO,
    OUT_OF_MEMORY,
    IO_INVALIDA,
    RECURSO_INVALIDO,
    INTERRUPTED_BY_USER,
    SUCCESS
} tipo_desalojo;

typedef enum {
    IO_GEN_SLEEP_INT,
    IO_STDIN_READ_INT,
    IO_STDOUT_WRITE_INT,
    IO_FS_CREATE_INT,
    IO_FS_DELETE_INT,
    IO_FS_TRUNCATE_INT,
    IO_FS_WRITE_INT,
    IO_FS_READ_INT
} tipo_operacion;


// Funciones para creacion de estructuras compartidas:
t_procesar_conexion* crear_procesar_conexion(char *server_name, int socket_cliente);



#endif //ESTRUCTURAS_SHARED_H_