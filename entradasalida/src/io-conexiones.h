#ifndef IO_CONEXIONES_H
#define IO_CONEXIONES_H

#include "io.h"

// Funciones para generar conexiones:
void interfaz_generar_conexiones_con();

// Funciones para generar conexiones con el kernel y la memoria:
void generar_conexiones_con_kernel();
void generar_conexiones_con_memoria();
void generar_conexion(int *socket, char *nombre_interfaz, char *ip, char *puerto);

#endif // IO_CONEXIONES_H