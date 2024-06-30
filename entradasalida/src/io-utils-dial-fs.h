#ifndef IO_UTILS_DIAL_FS_H
#define IO_UTILS_DIAL_FS_H

#include <stdio.h>
#include <sys/mman.h>
#include <dirent.h>
#include <commons/collections/list.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/string.h>
#include <utils/logger.h>
#include <utils/shared.h>

#include "io-estructuras.h"
#include "io-utils.h"

// Funciones de inicialización
FILE *iniciar_archivo_bloques(t_interfaz *interfaz, char *modo_de_apertura);
FILE *iniciar_archivo_bitmap(t_interfaz *interfaz, char *modo_de_apertura);
t_config *iniciar_archivo_config(t_interfaz *interfaz, char *name_file);
t_bitarray *iniciar_bitmap(t_interfaz *interfaz, char *modo_de_apertura);
t_config *iniciar_archivo_metadata(t_interfaz *interfaz, char *name_file);
FILE *iniciar_archivo(t_interfaz *interfaz, char *name_file, char *modo_de_apertura);

// Funciones de manipulación de bloques y bitmap
void set_bloque_libre_archivo_metadata(t_config *archivo_metadata, int bloque_libre);
void setear_bloque_ocupado(t_bitarray *bitmap, int bloque_libre);
int preguntar_si_bloque_esta_ocupado(t_bitarray *bitmap, int bloque_a_consultar);
int buscar_bloque_libre(t_bitarray *bitmap, t_interfaz *interfaz);
void liberar_bloques_usados(t_bitarray *bitmap, int bloque_inicial, int tam_archivo);
void inicializar_bloques_vacios(t_bitarray *bitmap, t_interfaz *interfaz);
int hay_bloques_suficientes(t_bitarray *bitmap, t_interfaz *interfaz, int cantidad_de_bloques_nuevos_necesarios);
int hay_bloques_contiguos(t_bitarray *bitmap, t_interfaz *interfaz, t_config *archivo_metadata, int cantidad_de_bloques_nuevos_necesarios);
void asignar_nuevos_bloques_desde_final(t_bitarray *bitmap, t_interfaz *interfaz, t_config *archivo_metadata, int cantidad_de_bloques_nuevos_necesarios);
void asignar_bloques_nuevos_desde_inicio(t_bitarray *bitmap, t_interfaz *interfaz, t_config *archivo_metadata, int cantidad_de_bloques_nuevos_necesarios);
int contar_bloques_libres_hasta(t_bitarray *bitmap, int desde_bloque, int hasta_bloque);
int calcular_bloque_final(t_interfaz *interfaz, t_config *archivo_metadata);
int calcular_cantidad_bloques_necesarios(t_interfaz *interfaz, int tam_a_establecer);
int calcular_cantidad_de_bloques_asignados(t_interfaz *interfaz, t_config *archivo_metadata);
void liberar_recuros_archivo(t_bitarray* bitmap, t_config* archivo_metadata);

// Funciones de manipulacion de metadata:
int get_tamanio_archivo(t_config *archivo_metadata);
int get_bloque_inicial(t_config *archivo_metadata);
t_config *get_archivo_config_from_args(t_interfaz *interfaz, t_list *argumentos);
void set_tamanio_archivo(t_config *archivo_metadata, int tam_a_establecer);
void set_bloque_inicial_archivo_metadata(t_config *archivo_metadata, int bloque_inicial);
int es_un_archivo_valido(t_config *archivo_metada);
void eliminar_metadata_archivo(t_config *archivo_metadata);
void crear_archivo(t_config *archivo_metada, t_interfaz *interfaz, char *name_file);
t_config *get_archivo_metadata(t_archivo_abierto *archivo_abierto);
void eliminar_archivo_de_archivos_abiertos(t_list *archivos_abiertos, char *name_file);
t_archivo_abierto *get_archivo_abierto(t_list *archivos_abiertos, char *name_file);
void set_bloque_inicial_en_archivo_abierto(int bloque_inicial, t_archivo_abierto *archivo_abierto);
void set_name_file_en_archivo_abierto(t_archivo_abierto *archivo_abierto, char *name_file);
void set_archivo_metada_en_archivo_abierto(t_archivo_abierto *archivo_abierto, t_config *archivo_metadata);
void agregar_a_archivos_abiertos(t_list *archivos_abiertos, t_config *archivo_metadata, char *name_file, int bloque_inicial);
int existe_archivo_abierto(t_list *archivos_abiertos, char *name_file);
t_list *traer_archivos_abiertos(t_interfaz *interfaz);

// Funciones de lectura y escritura
void procesar_operacio_de_lectura_o_escritura(t_config *archivo_metadata, t_list *argumentos, int *bloque_inicial, int *tamanio_archivo, int *bytes_a_leer, int *offset, int *dirreccion_fisica);
void leer_contenido_desde_bloques(FILE *bloques, int bloque_inicial, int offset, int bytes_a_leer, unsigned char *buffer, t_interfaz *interfaz);
void escribir_contenido_en_bloques(FILE *bloques, int bloque_inicial, int offset, int tamanio_archivo, unsigned char *contenido, int bytes_a_escribir, t_interfaz *interfaz);

// Funciones de utilidad
char *build_full_path(t_interfaz *interfaz, const char *name_file);
int ordenar_por_bloque_inicial(t_archivo_abierto *archivo_abierto, t_archivo_abierto *otro_archivo_abierto);
char *get_name_file(t_archivo_abierto *archivo_abierto);
char *get_modo_de_apertura(t_list *archivos_abiertos);

#endif // IO_UTILS_DIAL_FS_H