#ifndef IO_VARIABLES_GLOBALES_H_
#define IO_VARIABLES_GLOBALES_H_

// Incluimos las bibliotecas necesarias:
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <commons/collections/list.h>
#include <commons/bitarray.h>

// Incluimos bibliotecas propias:
#include "io-archivos-abiertos.h"
#include "io-archivos.h"
#include "io-estructuras.h"

// Definimos las variables globales:
extern t_list *archivos_abiertos;
extern t_bitarray *bitarray;
extern FILE *archivo_bitmap;
extern FILE *archivo_bloques;
extern t_interfaz *interfaz;

// Funcion para cerrar programa:
void cerrar_programa(int);

// Funcion para configurar la senial de cierre:
void configurar_senial_cierre();

#endif 

