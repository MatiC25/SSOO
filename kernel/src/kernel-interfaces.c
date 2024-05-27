#include "kernel-interfaces.h"

// Faltan semaforos, y ademas el dicccionario:
sem_t semaforo_diccionario;
sem_t sem_diccionario;
sem_t semaforo_blocked_ready;
t_dictionary* interface;
t_list* cola_new;
t_list* cola_ready;
t_list* cola_block;

interface_io *initialize_interface() {
    interface_io *interface = malloc(sizeof(interface_io));
    interface->connected = 0;
    interface->SOCKET = -1;
    interface->process_blocked = queue_create();
    sem_init(&interface->semaforo_blocked, 0, 1);

    return interface;
}

void set_name_interface(interface_io *interface, char *name) {
    interface->name = name;
}

void add_interface_to_dict(interface_io *interfaces, char *key) { 
    dictionary_put(interface, key, interfaces); //Noc si esta bien el diccecionario
}

interface_io *get_interface_from_dict(char *key) {
    return dictionary_get(interface, key); //Noc si esta bien el diccecionario
}

void handle_new_interface(void* arg) {
    int socket_servidor = (intptr_t) arg;

    while(1) {
        pthread_t accept_interfaces_thread;
        int socket_cliente = esperar_cliente("Interfaces",socket_servidor);

        pthread_create(&accept_interfaces_thread, NULL, (void*)manage_interface, (void *)(intptr_t) socket_cliente); // Cambiado de "administrar_interfaz"
        pthread_detach(accept_interfaces_thread);
    }
}

void manage_interface(void *socket_cliente) { 
    int socket = (intptr_t) socket_cliente;

    while(1) {
        op_code codigo_operacion = recibir_operacion(socket);

        switch(codigo_operacion) {
            case HANDSHAKE:
            generate_handshake_response(socket);
            break;
            case CREAR_INTERFAZ:
            create_interface(socket);
            case OPERACION_IO: //PARA DESALOJAR LA INTERFAZ
            t_pcb* proceso = recibir_proceso(socket_cliente);
            manejar_peticion_io(proceso, codigo_operacion);
            break;
        }
    }
}
void generate_handshake_response(int socket){
    enviar_mensaje("Hola I/O soy kenrel",socket);
}

void manejar_peticion_io(t_pcb* proceso, int codigo_operacion){
    interface_io* interfaz;

    if (!interfaz->connected){
        proceso->estado = EXITT;
        free(proceso);
    }
    sem_wait(&interfaz->semaforo_blocked);
    queue_push(interfaz->process_blocked, proceso);
    sem_post(&interfaz->semaforo_blocked);
}


t_pcb* recibir_proceso(int* socket){
    t_pcb* proceso = malloc(sizeof(t_pcb));
    if(recv(socket,proceso,sizeof(t_pcb),0)<= 0){
        free(proceso);
        return NULL;
    }
    return proceso;
}

void create_interface(int socket) {
    char *interface_name = recibir_mesj(socket);
    interface_io *interface = initialize_interface();

    // Seteamos nombre de la interfaz:
    set_name_interface(interface, interface_name);

    // Agregamos interfaz al diccionario:
    sem_wait(&semaforo_diccionario);
    add_interface_to_dict(interface, interface_name);
    sem_post(&semaforo_diccionario);

    create_consumer_thread(&interface_name);
}

char* recibir_mesj(int socket_cliente){
	int size;
	char *buffer = recibir_buffer(&size, socket_cliente);
	return buffer;
}


void create_consumer_thread(char *interface_name) {
    pthread_t consumer_thread;

    pthread_create(&consumer_thread, NULL, (void*) producer_pcbs_blockeds, (void *) interface_name);
    pthread_join(consumer_thread, NULL);
}

void producer_pcbs_blockeds(void *args) {
    sem_wait(&sem_diccionario);
    char *interface_name = (char *) args;
    interface_io *interface = get_interface_from_dict(interface_name);
    sem_post(&sem_diccionario);

    while(1) {
        
        if(queue_size(interface->process_blocked) > 0) {
            int response;

            sem_wait(&interface->semaforo_blocked);
            t_pcb* proceso = (intptr_t)queue_pop(interface->process_blocked); // Sacamos el primer PCB de la cola de bloqueados

            // Esperamos respuesta de la IO:
            response = send_message_to_interface(interface->SOCKET);

            if(response == 1) { // Si la IO termino de ejecutar la operacion
                sem_wait(&semaforo_blocked_ready);
                cambiar_block_a_ready(proceso);
                sem_post(&semaforo_blocked_ready);
            } else {
                queue_push(interface->process_blocked,proceso);
                sem_post(&interface -> semaforo_blocked); // Si no termino, volvemos a encolar el PCB
            }
        }

        sem_post(&semaforo_diccionario);
    }
}

int send_message_to_interface(int socket){
    char* mensaje= "Hola soy kernel";
    if(send(socket,mensaje,strlen(mensaje), 0) < 0 ){
        log_error(logger,"Error al enviar mensaje a la interfaz");
        return -1;
    }
    return;
}

void cambiar_block_a_ready(t_pcb*  proceso){
   pthread_mutex_lock(&mutex_estado_block);
    t_pcb* proceso_removido = list_remove_by_condition(cola_block, (bool (*)(void*)) recibir_pid);
   pthread_mutex_unlock(&mutex_estado_block);
   proceso_removido->estado = READY;
   sem_wait(&semaforo_blocked_ready);
   list_add(cola_ready,proceso_removido);
   sem_post(&semaforo_blocked_ready);
}

bool recibir_pid(t_pcb* proceso, int* pid){
    return proceso->pid == *pid;
}