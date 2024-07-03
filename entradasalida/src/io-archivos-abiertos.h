#ifndef IO_ARCHIVOS_ABIERTOS_H_
#define IO_ARCHIVOS_ABIERTOS_H_

#include <dirent.h>
#include <commons/config.h>
#include <utils/logger.h>
#include <utils/shared.h>
#include <commons/collections/list.h>

#include "io-estructuras.h"
#include "io-archivos-metadata.h"
#include "io-utils.h"

// Funcione para traer los archivos ya abiertos:
t_list *obtener_archivos_ya_abiertos(t_interfaz *interfaz);

// Funciones para setear los datos del archivo abierto:
void set_archivo_metada_en_archivo_abierto(t_archivo_abierto *archivo_abierto, t_config *archivo_metadata);
void set_name_file_en_archivo_abierto(t_archivo_abierto *archivo_abierto, char *name_file);