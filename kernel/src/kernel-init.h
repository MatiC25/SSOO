#ifndef KERNEL_INIT_H
#define KERNEL_INIT_H

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <utils/logger.h>
#include <kernel.h>
#include "kernel-estructuras.h"
#include <utils/socket.h>

typedef enum { //podemos juntar todos los procesos 

//Agregar operacions
// ----------------
} in_code;

void iniciar_modulo_kernel(int socket_servidor);
void aceptar_interfaces(int socket_servidor);
void iniciar_planificacion(); 
void manejar_peticion_con_memoria(); 
void manejar_peticion_con_cpu();
void* aceptar_conexiones(void* arg);



#endif //KERNEL_INIT_H