#ifndef IO_ARCHIVOS_H
#define IO_ARCHIVOS_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <commons/string.h>

#include "io-estructuras.h"
#include "io-utils.h"

// Funciones para abrir archivos:
FILE *abrir_archivo_bloques(t_interfaz *interfaz, char *modo_de_apertura);
FILE *abrir_archivo_metadata(t_interfaz *interfaz, char *name_file, char *modo_de_apertura);
FILE *abrir_archivo_bitmap(t_interfaz *interfaz, char *modo_de_apertura);

// Funciones auxiliares:
FILE *abrir_archivo(char *path, char *modo_de_apertura);
FILE *persistir_archivo(t_interfaz *interfaz, char *name_file, char *modo_de_apertura, int tamanio_archivo);

// Funciones de utilidad:
int tiene_extension(const char *name_file, const char *extension);

#endif