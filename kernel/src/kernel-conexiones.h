#ifndef KERNEL_CONEXIONES_H
#define KERNEL_CONEXIONES_H

#include <utils/logs.h>
#include <utils/shared.h>
#include <utils/socket.h>

#include "kernel-config.h"

// Funciones para manejar conexiones:
void generar_conexiones_con();
void generar_conexiones_con_cpu();
void generar_conexion_con_memoria();

#endif // KERNEL_CONEXIONES_H