#ifndef IO_BITMAP_H
#define IO_BITMAP_H

#include <commons/bitarray.h>

#include "io-estructuras.h"
#include "io-archivos.h"
#include "io-utils.h"

// Funcion para crear un bitmap:
t_bitarray *crear_bitmap(t_interfaz *interfaz, char *modo_de_apertura);

// Funcion para inicializar un bitmap:
void inicializar_bitmap(t_bitarray *bitmap);

// Funcion para obtener un bloque libre:
int obtener_bloque_libre(t_bitarray *bitmap, t_interfaz *interfaz);

#endif