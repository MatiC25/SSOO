#ifndef IO_DIALS_FS_H
#define IO_DIALS_FS_H

// Incluye las biblotecas externas necesarias:
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <utils/logger.h>

// Incluye las bibliotecas internas necesarias:
// #include "io-estructuras.h"
#include "io-utils-dial-fs.h"
#include "io-protocolo.h"

// Operaciones de DIAL-FS:
void operacion_create_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos);
void operacion_read_file(t_interfaz *interfaz, FILE *bloques, t_list *argumentos);
void operacion_write_file(t_interfaz *interfaz, FILE *bloques, t_list *argumentos);
void operacion_delete_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos);

#endif // IO_DIALS_FS_H