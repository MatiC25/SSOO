#include "kernel-interfaces.h"

t_dictionary *interfaces;
sem_t semaforo_interfaces;

// Funciones de manejo de interfaz desde el lado del kernel:

void handle_new_interface(void *arg) {
    int socket_kernel = (int) arg;

    while(1) {
        int socket_cliente_nueva_interfaz = esperar_cliente("KENEL", socket_kernel);

        if(socket_cliente_nueva_interfaz != -1) {
            pthread_t hilo_nueva_interfaz;

            pthread_create(&hilo_nueva_interfaz, NULL, (void *) manage_interface, (void *) socket_cliente_nueva_interfaz);
            pthread_detach(&hilo_nueva_interfaz);
        }
    }
}

void manage_interface(void *socket_cliente) {
    int socket = (int) socket_cliente;
    int execute = 1;

    while(execute) {
        op_code operacion = recibir_operacion(socket);

        switch(operacion) {
            case NEW_INTERFACE:
                create_interface(socket);
                execute = 0;
                break;
            default:
                log_error(logger, "Operacion invalida!");
                break;
        }
    }
}

// Funciones de consumidores:

void create_consumer_thread(char *interface_name) {
    pthread_t consumer_thread;

    pthread_create(&consumer_thread, NULL, (void*) consumers_pcbs_blockeds, (void *) interface_name);
    pthread_join(consumer_thread, NULL);
}

void consumers_pcbs_blockeds(void *args) {
    sem_wait(&semaforo_interfaces);
    char *interface_name = (char *) args;
    interface_io *interface = get_interface_from_dict(interface_name);
    sem_post(&semaforo_interfaces);

    while(1) {
        // Esperamos que la interfaz no este bloqueada:
        sem_wait(&interface->semaforo_used);
        sem_wait(&interface->size_blocked); // Esperamos que haya procesos bloqueados

        // Sacamos el primer proceso de la cola de bloqueados:
        t_pcb *pcb = queue_pop(interface->process_blocked);
        t_list *args = queue_pop(interface->args_process); // Obtenemos los argumentos del proceso

        // Obtenemos el socket de la interfaz:
        int socket_with_interface = get_socket_interface(interface);
        int response = 0;
        
        // Esto tenemos que cambiarlo, es solo para probar:
        if(interface->tipo == GENERICA)
            send_message_to_generic_interface(socket_with_interface, args, response);
        
        if(response == 1)
            mover_procesos_de_bloqueado_a_ready(pcb);
        else
            // No sé que vamos hacer en este caso!

        sem_post(&interface->semaforo_used);
    }
}

// Funciones manipuladoras de interfaz:

interface_io *initialize_interface() {
    interface_io *interface = malloc(sizeof(interface_io));
    interface->socket_interface = -1;
    interface->process_blocked = queue_create();
    sem_init(&interface->semaforo_used, 0, 1);
    sem_init(&interface->size_blocked, 0, 0);

    return interface;
}

void create_interface(int socket) {
    tipo_interfaz tipo;
    char *interface_name;

    // Inicializamos interfaz:
    interface_io *interface = initialize_interface();

    // Recibimos el nombre de la interfaz y el tipo:
    recibir_interfaz(&interface_name, &tipo, socket);

    // Seteamos nombre de la interfaz:
    set_name_interface(interface, interface_name);

    // Seteamos las operaciones validas:
    set_valid_operations(interface, tipo);

    // Seteamos socket de la interfaz:
    set_socket_interface(interface, socket);

    // Agregamos interfaz al dccionario:
    sem_wait(&semaforo_interfaces);
    add_interface_to_dict(interface, interface_name);
    sem_post(&semaforo_interfaces);

    create_consumer_thread(&interface_name);
}

// Funciones de operaciones basicas de interfaz:

void set_name_interface(interface_io *interface, char *name) {
    interface->name = name;
}

int get_socket_interface(interface_io *interface) {
    return interface->socket_interface;
}

void set_socket_interface(interface_io *interface, int socket) {
    interface->socket_interface = socket;
}

void add_interface_to_dict(interface_io *interfaces, char *key) { 
    dictionary_put(interfaces, key, interfaces);
}

interface_io *get_interface_from_dict(char *key) {
    return dictionary_get(interfaces, key); 
}

void set_valid_operations(interface_io *interface, tipo_interfaz tipo) {
    interface->operaciones_validas = cargar_configuraciones_operaciones(tipo);
}


consulta_existencia_interfaz(interface_io *interface) {
    return interface != NULL;
}

int consulta_interfaz_para_aceptacion_de_operacion(interface_io *interface) {
    tipo_operacion operacion = operacion_a_realizar(config_kernel->SOCKET_DISPATCH);
    if(acepta_operacion_interfaz(interface, operacion)) 
        return 1; // Operacion valida
    else 
        return 0; // Operacion invalida
}

int acepta_operacion_interfaz(interface_io *interface, tipo_operacion operacion) {
    tipo_operacion operaciones[] = {
        IO_GEN_SLEEP_INT,
        IO_STDIN_READ_INT,
        IO_STDOUT_WRITE_INT,
        IO_FS_CREATE_INT,
        IO_FS_DELETE_INT,
        IO_FS_TRUNCATE_INT,
        IO_FS_WRITE_INT,
        IO_FS_READ_INT
    };

    tipo_interfaz tipo = interface->tipo;
    
    if(tipo == GENERICA)
        return operacion == operaciones[0];
    else if(tipo == STDIN)
        return operacion == operaciones[1];
    else if(tipo == STDOUT)
        return operacion == operaciones[2];
    else if(tipo == DIALFS)
        return operacion == operaciones[3] || operacion == operaciones[4] || operacion == operaciones[5] || operacion == operaciones[6] || operacion == operaciones[7];
    else
        return 0; // Operacion invalida
}

// Funciones para pedir operaciones a la interfaz:

void send_message_to_generic_interface(int socket, t_list *args, int *response) {
    char *tiempo_sleep = list_get(args, 0);
    int tiempo = atoi(tiempo_sleep);

    send(socket, &tiempo, sizeof(int), 0);
    recv(socket, response, sizeof(int), 0);

    return;
}

// Funciones para recibir mensajes de la interfaz:

tipo_operacion operacion_a_realizar(int socket) {
    tipo_operacion operacion;

    // Recibimos operacion a realizar desde el CPU:
    recv(socket, &operacion, sizeof(tipo_operacion), 0);

    return operacion;
}

tipo_interfaz recibir_tipo_interfaz(int socket) {
    tipo_interfaz tipo;
    
    // Recibimos tipo de interfaz desde el CPU:
    recv(socket, &tipo, sizeof(tipo_interfaz), 0);

    return tipo;
}

// Funciones de auxiliares:
void recibir_interfaz(char **interface_name, tipo_interfaz *tipo, int socket) {
    int size = 0;
    int desplazamiento = 0;
    int tamanio;
    void *buffer = recibir_buffer(&size, socket);
    
    memcpy(tipo, buffer + desplazamiento, sizeof(tipo_interfaz));
    desplazamiento += sizeof(tipo_interfaz);
    
    recibir_nombre_interfaz(interface_name, buffer, &desplazamiento, &tamanio);
}


void recibir_nombre_interfaz(char **interface_name, void *buffer, int *desplazamiento, int *size) { 
    memcpy(*size, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    *interface_name = malloc(tamanio);
    memcpy(*interface_name, buffer + desplazamiento, tamanio);
    *desplazamiento += *size; 
}