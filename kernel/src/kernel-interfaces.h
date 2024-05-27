#ifndef KERNEL_INTERFACES_H
#define KERNEL_INTERFACES_H

#include <utils/logger.h>
#include <utils/estructuras_compartidas.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <utils/socket.h>
#include <stdint.h>
#include "kernel-estructuras.h"

extern sem_t semaforo_diccionario;
extern sem_t sem_diccionario;
extern sem_t semaforo_blocked_ready;
extern t_dictionary* interface;
extern t_list* cola_new;
extern t_list* cola_ready;
extern t_list* cola_block;

typedef struct {
    char* name;
    int SOCKET;
    int connected; // Sirve para verificar si una I/O esta conectada
    t_queue *process_blocked; // Vamos a tener una cola de pid´s
    sem_t semaforo_blocked;
} interface_io;



// Funciones para inicializar:
interface_io *initialize_interface();
void set_name_interface(interface_io *interface, char *name);
void add_interface_to_dict(interface_io *interfaces, char *key);
interface_io *get_interface_from_dict(char *key);
void handle_new_interface(void* arg);
void manage_interface(void *socket_cliente);
void generate_handshake_response(int socket);
void create_interface(int socket);
t_pcb* recibir_proceso(int* socket);
void create_consumer_thread(char *interface_name);
void producer_pcbs_blockeds(void *args);
void manejar_peticion_io(t_pcb* proceso, int codigo_operacion);
char* recibir_mesj(int socket_cliente);
void cambiar_block_a_ready(t_pcb*  proceso);
int send_message_to_interface(int socket);
bool recibir_pid(t_pcb* proceso, int* pid);


#endif // KERNEL_INTERFACES_H