#ifndef ESTRUCTURAS_SHARED_H_
#define ESTRUCTURAS_SHARED_H_

#include <commons/log.h>
#include <stdint.h>

typedef struct {
    t_log* logger;
    char* server_name;
    int socket_cliente;
} t_procesar_conexion;

typedef struct {
    t_log* logger;
    char* server_name;
    int socket_servidor;
} t_procesar_server;

typedef struct { // Registro de la CPU
    uint32_t AX;
    uint32_t BX;
    uint32_t CX;
    uint32_t DX;
} t_registros_cpu;

typedef struct { // PCB de un proceso
    int pid;
    int program_counter;
    t_registros_cpu registros;
    char** archivos_abiertos; 
}t_pcb;

// Funciones para creacion de estructuras compartidas:
t_procesar_conexion* crear_procesar_conexion(t_log *logger, char *server_name, int socket_cliente);

#endif 