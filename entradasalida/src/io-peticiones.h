#ifndef IO_PETICIONES_H
#define IO_PETICIONES_H

//#include "io-estructuras.h"
#include <readline/readline.h>
#include <utils/estructuras_compartidas.h>
#include <utils/logger.h>

// Incluye las estructuras necesarias:
#include "io-config.h"
#include "io-estructuras.h"
#include "io-utils.h"
#include "io-protocolo.h"


// #define BLOCK_SIZE 1024
// #define BLOCK_COUNT 1000
// #define PATH_BLOQUES "bloques.dat"
// #define PATH_BITMAP "bitmap.dat"
// #define PATH_METADATA "./metadata/"

// typedef struct {
//     int bloque_inicial;
//     int tamanio_archivo;
// } metadata_archivo_t;

// Funciones para enviar peticiones:
void interfaz_recibir_peticiones(t_interfaz* interfaz);

// Funciones para ejecutar operaciones:
void ejecutar_operacion_generica(t_interfaz * interfaz);
void ejecutar_operacion_stdin(t_interfaz *interfaz);
void ejecutar_operacion_stdout(t_interfaz *interfaz);
//void ejecutar_operacion_dialfs(t_interfaz *interfaz);

// //instrucciones de las interfaces diaslFS
// void crear_fs(t_interfaz *interfaz);
// void borrar_fs(t_interfaz *interfaz);
// void truncar_fs(t_interfaz *interfaz);
// void escribir_fs(t_interfaz *interfaz);
// void leer_fs(t_interfaz *interfaz);

// //funciones auxiliares
// int obtener_bloque_libre();
// void marcar_bloque_ocupado(int bloque);
// void crear_metadata_archivo(char* nombre, int bloque_inicial, int tamanio);
// int leer_metadata_archivo(char* nombre, int* bloque_inicial, int* tamanio);
// void liberar_bloques(int bloque_inicial, int cantidad);
// void eliminar_metadata_archivo(char* nombre);
// void actualizar_metadata_archivo(char* nombre, int bloque_inicial, int tamanio);
// void escribir_datos_en_bloques(int bloque_inicial, int direccion, char* datos, int cantidad);
// void leer_datos_de_bloques(int bloque_inicial, int direccion, char* datos, int cantidad);
// void compactar_fs();

#endif // IO_PETICIONES_H