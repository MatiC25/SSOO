#ifndef IO_PROTOCOLOS_H
#define IO_PROTOCOLOS_H

// Incluye las biblotecas externas necesarias:
#include <utils/estructuras_compartidas.h>
#include <utils/protocolo.h>
#include <utils/logger.h>

// Incluye las estructuras necesarias:
#include "io-config.h"
#include "io-estructuras.h"
#include "io-utils.h"

// Funciones para enviar mensajes al kernel
void send_respuesta_a_kernel(int respuesta, t_interfaz * interfaz);

// Funciones para recibir mensajes del kernel
int recibir_entero(t_interfaz * interfaz);

// Funciones para enviar mensajes a la memoria:
void send_mensaje_a_memoria(t_interfaz * interfaz, char *mensaje);
void send_bytes_a_leer(t_interfaz *interfaz, int direccion_fisica, int bytes_a_mostrar);
void send_bytes_a_grabar(t_interfaz * interfaz, int direccion_fisica, char *bytes, int bytes_a_leer);

// Funciones para recibir mensajes de la memoria
void rcv_bytes_a_leer(t_interfaz *interfaz, int bytes_a_mostrar, char **bytes);

#endif // IO_PROTOCOLOS_H