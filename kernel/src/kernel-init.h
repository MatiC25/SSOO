#ifndef KERNEL_INIT_H
#define KERNEL_INIT_H

// #include <commons/collections/list.h>
// #include <commons/collections/queue.h>
// #include <utils/logger.h>
// #include <kernel.h>
// #include <stdint.h>
// #include <pthread.h>
// #include <utils/socket.h>

// #include "consola.h"
// #include "kernel-estructuras.h"
// #include "kernel-interfaces.h"
// #include "planificacion.h"
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <utils/logger.h>
#include <kernel.h>
#include <stdint.h>
#include <pthread.h>
#include <utils/socket.h>
#include "kernel-estructuras.h"
#include "kernel-interfaces.h"
#include "planificacion.h"

void iniciar_modulo_kernel(int socket_servidor);
void aceptar_interfaces(int socket_servidor);
void iniciar_planificacion(); 
void manejar_peticion_con_memoria();
void manejar_peticion_con_cpu();
void inicializar_lista();


#endif //KERNEL_INIT_H