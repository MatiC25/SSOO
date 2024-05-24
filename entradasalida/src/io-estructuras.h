#ifndef IO_ESTRUCTURAS_H
#define IO_ESTRUCTURAS_H

#include <utils/estructuras_compartidas.h>

typedef struct {
    char *nombre;
    tipo_interfaz tipo;
    t_config_io *config;
} t_interfaz;

typedef struct {
    int TIEMPO_UNIDAD_UNIDAD;
    char *IP_KERNEL;
    char *PUERTO_KERNEL;
    char *IP_MEMORIA;
    char *PUERTO_MEMORIA;
    char *PATH_BASE_DIALFS;
    int BLOCK_SIZE;
    int BLOCK_COUNT;
    int RETRASO_COMPACTACION;
} t_config_io;

#endif