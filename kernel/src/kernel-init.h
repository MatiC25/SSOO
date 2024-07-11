#ifndef KERNEL_INIT_H
#define KERNEL_INIT_H

// #include "planificacion.h"
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <utils/logger.h>
#include <kernel.h>
#include <stdint.h>
#include <pthread.h>
#include <utils/socket.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "kernel-peticiones-dispatch.h"
#include "kernel-estructuras.h"
#include "kernel-interfaces.h"
#include "planificacion.h"
#include "consola.h"
#include "kernel-utils-interfaces.h"

extern volatile sig_atomic_t terminate_program;

void iniciar_modulo_kernel(int socket_servidor);
void aceptar_interfaces(int socket_servidor);
void iniciar_planificacion(); 
void manejar_peticion_con_memoria();
void manejar_peticion_con_cpu();
void inicializar_lista();
void finalizar_programa();
void close_signal();
void cerrar_programa();
void liberar_interface_io(interface_io *interface);
void liberar_lista_de_argumentos(t_list *args);

#endif //KERNEL_INIT_H