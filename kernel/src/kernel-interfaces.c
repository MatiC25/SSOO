#include "kernel-interfaces.h"
t_dictionary *interfaces = NULL;
t_dictionary *args_consumers = NULL;
sem_t semaforo_interfaces;

// Funciones de manejo de interfaz desde el lado del kernel:

void handle_new_interface(void* arg) {
    int socket_servidor = (intptr_t) arg;

    while(1) {
        pthread_t accept_interfaces_thread;
        int socket_cliente = esperar_cliente("Interfaces", socket_servidor);

        pthread_create(&accept_interfaces_thread, NULL, (void*)manage_interface, (void *)(intptr_t) socket_cliente); // Cambiado de "administrar_interfaz"
        pthread_detach(accept_interfaces_thread);
    }
}

void manage_interface(void *socket_cliente) { 
    int socket = (int) socket_cliente;
    int seguir = 1;

    while(seguir) {
        op_code codigo_operacion = recibir_operacion(socket);

        switch(codigo_operacion) {
            case HANDSHAKE:
                recibir_handshake(socket);
            break;
            case CREAR_INTERFAZ:
                create_interface(socket);
                seguir = 0;
            break;
            default:
                seguir = 0;
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
        //t_list *args = get_args(pcb); // Obtenemos los argumentos del proceso

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
    interface->SOCKET = -1;
    interface->process_blocked = queue_create();
    sem_init(&interface->semaforo_used, 0, 1);
    sem_init(&interface->size_blocked, 0, 0);

    return interface;
}

void create_interface(int socket) {
    char *interface_name;
    tipo_interfaz tipo;

    // Inicializamos interfaz:
    interface_io *interface = initialize_interface();

    // Recibimos los bytes de la interfaz:
    recibir_interfaz(interface_name, &tipo, socket);

    // Seteamos nombre de la interfaz:
    set_name_interface(interface, interface_name);

    // Seteamos las operaciones validas:
    set_valid_operations(interface, tipo);

    // Seteamos socket de la interfaz:
    set_socket_interface(interface, socket);

    printf("%s",interface_name);

    // Agregamos interfaz al dccionario:
    sem_wait(&semaforo_interfaces);
    add_interface_to_dict(interface, interface_name);
    sem_post(&semaforo_interfaces);

    create_consumer_thread(interface_name);
}

// Funciones de operaciones basicas de interfaz:

t_list *cargar_configuraciones_operaciones(tipo_interfaz tipo) {
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

    // Creamos la lista de operaciones:
    t_list *lista_operaciones = list_create();
    agregar_operaciones(lista_operaciones, operaciones, tipo);

    return lista_operaciones;
}

void agregar_operaciones(t_list *lista_operaciones, tipo_operacion operaciones[], tipo_interfaz tipo) {
    if(tipo == GENERICA) {
        list_add(lista_operaciones, IO_STDIN_READ_INT);
    // } else if(tipo == STDIN) {
    //     list_add(lista_operaciones, IO_STDOUT_WRITE_INT);
    // } else if(tipo == STDOUT) {
    //     list_add(lista_operaciones, IO_FS_CREATE_INT);
    // } else if(tipo == DIALFS) {
    //     for(int i = 3; i < 7; i++) {
    //         list_add(lista_operaciones, operaciones[i]);
    //     }
    }
}


// Metodos de interfaz:

void set_name_interface(interface_io *interface, char *name) {
    interface->name = name;
}

int get_socket_interface(interface_io *interface) {
    return interface->SOCKET;
}

void set_socket_interface(interface_io *interface, int socket) {
    interface->SOCKET = socket;
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

// Funciones para pedir operaciones a la interfaz:

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
    int size_operaciones_validas = list_size(interface -> operaciones_validas);
    for(int i = 0; i < size_operaciones_validas; i++)
        if(operacion == list_get(interface -> operaciones_validas, i))
            return 1; // Operacion valida
    
    return 0; // Operacion invalida
}

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

void recibir_interfaz(char *interface_name, tipo_interfaz *tipo, int socket) {
    int size = 0;
    int desplazamiento = 0;
    int tamanio;
    void *buffer = recibir_buffer(&size, socket);

    memcpy(&tipo, buffer + desplazamiento, sizeof(tipo_interfaz));
    desplazamiento += sizeof(tipo_interfaz);

    memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    interface_name = malloc(tamanio);
    memcpy(interface_name, buffer + desplazamiento, tamanio);
}

// Funciones de auxiliares: