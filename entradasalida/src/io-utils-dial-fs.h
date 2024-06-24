#ifndef IO_UTILS_DIAL_FS_H
#define IO_UTILS_DIAL_FS_H

#include <stdio.h>
#include <sys/mman.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include <utils/logger.h>
#include <utils/shared.h>

#include "io-estructuras.h"
#include "io-utils.h"

// Funciones de inicialización
FILE *iniciar_archivo_bloques(t_interfaz *interfaz);
FILE *iniciar_archivo_bitmap(t_interfaz *interfaz);
t_config *iniciar_archivo_config(t_interfaz *interfaz, char *name_file);
t_bitarray *iniciar_bitmap(t_interfaz *interfaz);
t_config *iniciar_archivo_metadata(t_interfaz *interfaz, char *name_file);

// Funciones de manipulación de bloques y bitmap
void set_bloque_libre_archivo_metadata(t_config *archivo_metadata, int bloque_libre);
void setear_bloque_ocupado(t_bitarray *bitmap, int bloque_libre);
int buscar_bloque_libre(t_bitarray *bitmap, t_interfaz *interfaz);
void liberar_bloques_usados(t_bitarray *bitmap, int bloque_inicial, int tam_archivo);
void inicializar_bloques_vacios(t_bitarray *bitmap, t_interfaz *interfaz);

// Funciones de lectura y escritura
void procesar_operacio_de_lectura_o_escritura(t_config *archivo_metadata, t_list *argumentos, int *bloque_inicial, int *tamanio_archivo, int *bytes_a_leer, int *offset, int *dirreccion_fisica);
void leer_contenido_desde_bloques(FILE *bloques, int bloque_inicial, int offset, int bytes_a_leer, unsigned char *buffer, t_interfaz *interfaz);
void escribir_contenido_en_bloques(FILE *bloques, int bloque_inicial, int offset, int tamanio_archivo, unsigned char *contenido, int bytes_a_escribir, t_interfaz *interfaz);

// Funciones de utilidad
FILE *iniciar_archivo(t_interfaz *interfaz, char *name_file);
char *build_full_path(t_interfaz *interfaz, const char *name_file);
int get_bloque_inicial(t_config *archivo_metadata);
int get_tamanio_archivo(t_config *archivo_metadata);
t_config *get_archivo_config_from_args(t_interfaz *interfaz, t_list *argumentos);
int es_un_archivo_valido(t_config *archivo_metada);

#endif // IO_UTILS_DIAL_FS_H