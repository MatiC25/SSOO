#ifndef IO_DIALS_FS_H
#define IO_DIALS_FS_H

// Incluye las biblotecas externas necesarias:
#include <commons/collections/bitarray.h>
#include <commons/collections/list.h>
#include <utils/logger.h>

// Incluye las bibliotecas internas necesarias:
#include "io-estructuras.h"
#include "io-operaciones-dial-fs.h"

// Operaciones de DIAL-FS:
void operacion_create_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos);
void operacion_read_file(t_interfaz *interfaz, FILE *bloques, t_list *argumentos);
void operacion_write_file(t_interfaz *interfaz, FILE *bloques, t_list *argumentos);
void operacion_delete_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos);