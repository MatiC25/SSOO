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
    uint8_t AX, BX, CX, DX;
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
    t_registro_cpu* registros;
    char *palabra;
}t_pcb;

// typedef enum {
//     IO,
//     FIN_QUANTUM,
//     WAIT,
//     SIGNAL,
//     FIN_EJECUCION,
// } tipo_desalojo;

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
// char *tipo_interfaz_to_string(tipo_interfaz tipo);

#endif //ESTRUCTURAS_SHARED_H_