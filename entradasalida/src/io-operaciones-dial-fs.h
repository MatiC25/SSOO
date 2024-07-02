#ifndef IO_DIALS_FS_H
#define IO_DIALS_FS_H

// Incluye las biblotecas externas necesarias:
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <utils/logger.h>

// Incluye las bibliotecas internas necesarias:
#include "io-utils-dial-fs.h"
#include "io-protocolo.h"
#include "io-compactacion.h"

// Operaciones de DIAL-FS:
void operacion_create_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos, t_list *archivos_abiertos);
void operacion_read_file(t_interfaz *interfaz, FILE *bloques, t_list *argumentos, t_list *archivos_abiertos);
void operacion_write_file(t_interfaz *interfaz, FILE *bloques, t_list *argumentos, t_list *archivos_abiertos);
void operacion_delete_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos, t_list *archivos_abiertos) ;
void operacion_truncate_file(t_interfaz *interfaz, FILE *bloques, t_bitarray *bitmap, t_list *argumentos, t_list *archivos_abiertos);

#endif // IO_DIALS_FS_H