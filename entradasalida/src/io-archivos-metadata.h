#ifndef IO_ARCHIVOS_METADATA_H_
#define IO_ARCHIVOS_METADATA_H_ 

#include <stdio.h>
#include <commons/config.h>
#include <utils/logger.h>
#include <utils/shared.h>

#include "io-estructuras.h"
#include "io-utils.h"
#include "io-archivos.h"

// Funciones para abrir los archivos:
t_config *abrir_archivo_metadata_config(t_interfaz *interfaz, char *name_file, char *modo_de_apertura);

// Funciones para validar los archivos:
int es_un_archivo_valido(t_config *archivo_metadata);