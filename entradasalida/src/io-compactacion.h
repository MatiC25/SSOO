#ifndef IO_COMPACTACION_H
#define IO_COMPACTACION_H

#include <utils/estructuras_compartidas.h>
#include <commons/bitarray.h>
#include <commons/config.h>
#include <commons/log.h>

#include "io-estructuras.h"
#include "io-utils-dial-fs.h"

void compactar_archivos(t_bitarray *bitmap, FILE *bloques, t_interfaz *interfaz, t_list *archivos_abiertos, t_config *archivo_metadata, int cantidad_de_bloques_nuevos_necesarios);