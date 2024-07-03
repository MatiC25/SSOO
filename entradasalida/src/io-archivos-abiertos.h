#ifndef IO_ARCHIVOS_ABIERTOS_H_
#define IO_ARCHIVOS_ABIERTOS_H_

#include <dirent.h>
#include <commons/config.h>
#include <utils/logger.h>
#include <utils/shared.h>

#include "io-estructuras.h"
#include "io-archivos-metadata.h"
#include "io-archivos.h"
#include "io-utils.h"

// Funcione para traer los archivos ya abiertos:
t_list *obtener_archivos_ya_abiertos(t_interfaz *interfaz);

// Funciones para setear los datos del archivo abierto:
void set_archivo_metada_en_archivo_abierto(t_archivo_abierto *archivo_abierto, t_config *archivo_metadata);
void set_name_file_en_archivo_abierto(t_archivo_abierto *archivo_abierto, char *name_file);
void set_nuevo_archivo_abierto(t_list *archivos_abiertos, char *name_file, t_config *archivo_metadata);

// Funciones para gettear los datos del archivo abierto:


// Funciones para verificar si existe ya un archivo:
int ya_esta_abierto(t_list *archivos_abiertos, char *nombre_archivo);

#endif