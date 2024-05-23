#ifndef ESTRUCTURAS_SHARED_H_
#define ESTRUCTURAS_SHARED_H_

#include <commons/log.h>
#include <stdint.h>
#include <utils/logger.h>

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
    uint32_t *SI; // Cambiado a puntero
    uint32_t *DI; // Cambiado a puntero
} t_registro_cpu;

typedef enum {
    NEW,
    READY,
    EXEC,
    WAIT,
    EXIT
} status_cod;

typedef struct {
    int pid;
    int program_counter; 
    status_cod estado;
    t_registro_cpu* registro;
    int quantum;
} t_pcb;

// Funciones para creacion de estructuras compartidas:
t_procesar_conexion* crear_procesar_conexion(char *server_name, int socket_cliente);



#endif //ESTRUCTURAS_SHARED_H_