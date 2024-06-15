#ifndef IO_PETICIONES_H
#define IO_PETICIONES_H

#include "io.h"

void interfaz_recibir_peticiones();
void ejecutar_operacion_generica();
void enviar_respuesta_a_kernel(int respuesta);
int recibir_tiempo();

#endif // IO_PETICIONES_H