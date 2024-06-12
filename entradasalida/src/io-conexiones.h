#ifndef IO_CONEXIONES_H
#define IO_CONEXIONES_H

// Incluye las bibliotecas necesarias:
#include <utils/socket.h>
#include <utils/logger.h>

// Incluye las estructuras necesarias:
#include "io-config.h"
#include "io-estructuras.h"
#include "io-utils.h"

// Funciones para generar conexiones:
void interfaz_generar_conexiones_con(t_interfaz* interfaz);

// Funciones para generar conexiones con el kernel y la memoria:
void generar_conexiones_con_kernel(t_interfaz* interfaz);
void generar_conexiones_con_memoria(t_interfaz* interfaz);
void generar_conexion(int *socket, char *nombre_interfaz, char *ip, char *puerto);

// Funciones: 
void interfaz_conectar(t_interfaz * interfaz);

#endif // IO_CONEXIONES_H