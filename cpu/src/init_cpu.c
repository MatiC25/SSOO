#include "init_cpu.h"

//CARGAMOS LOS DATOS DEL CONFIG
int cargar_configuraciones(t_config_cpu* config_cpu, t_log* logger) {
    t_config* config = config_create("cpu.config");

    if(config_cpu == NULL) {
        log_error(logger, "No se pudo cargar la configuracion del filesystem");

        return -1;
    }

    char* configuraciones[] = {
        "IP_MEMORIA",
        "PUERTO_MEMORIA",
        "PUERTO_ESCUCHA_DISPATCH",
        "PUERTO_ESCUCHA_INTERRUPT",
        "CANTIDAD_ENTRADAS_TLB",
        "ALGORITMO_TLB",
        NULL
    };

    if(!tiene_todas_las_configuraciones(config, configuraciones)) {
        log_error(logger, "No se pudo cargar la configuracion del cpu");

        return -1;
    }
    

    copiar_valor(&config_cpu->ip_memoria, config_get_string_value(config, "IP_MEMORIA"));

    config_cpu->puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
    config_cpu->puerto_escucha_dispatch = config_get_int_value(config, "PUERTO_ESCUCHA_DISPATCH");
    config_cpu->puerto_escucha_interrupt = config_get_int_value(config, "PUERTO_ESCUCHA_INTERRUPT");

    log_info(logger, "Configuraciones cargadas correctamente");
    config_destroy(config);

    return 1;
}

// GENERAMOS CONEXIONES DE CLIENTE A SERVER MEMORIA
int generar_conexiones(t_log* logger, t_config_cpu* config_cpu, int* md_memoria) {
    
    // no es el puerto correcto, pero debo levantar por config
    char* puerto_memoria = string_itoa(config_cpu->puerto_memoria);
    char* ip_memoria = config_cpu->ip_memoria;



    *md_memoria = crear_conexion(logger, "MEMORIA", ip_memoria, puerto_memoria); // No harcodearlo! Sino leerlo de kernel.config
    // No existe valores por referencias en C! Primero le sacamos la direccion de memoria a la variable, y despues con *variable asignamos el nuevo valor!

    return (*md_memoria != 0) ? 1 : -1; // Aca pregunto por el nuevo valor!
}

// CREAMOS SERVIDOR PARA EL CLIENTE KERNEL
int crear_servidores(t_log* logger, t_config_cpu* config_cpu, int* md_cpu_ds, int* md_cpu_it) {
    
    char* puerto_dispatch = string_itoa(config_cpu->puerto_escucha_dispatch); // Convierte un int a una cadena de char
    char* puerto_interrupt = string_itoa(config_cpu->puerto_escucha_interrupt);
    
    *md_cpu_ds = iniciar_servidor(logger, "DISPATCH", NULL, puerto_dispatch); // Guarda ID del socket
    *md_cpu_it = iniciar_servidor(logger, "INTERRUPT", NULL, puerto_interrupt); 

    return (*md_cpu_ds != 0 && *md_cpu_it != 0) ? 1 : -1;
}

void iniciar_modulo(t_log* logger_cpu, t_config_cpu* config_cpu) {

    int md_cpu_ds = 0;
    int md_cpu_it = 0;

    if(crear_servidores(logger_cpu, config_cpu, &md_cpu_ds, &md_cpu_it) != 1) {
        log_error(logger_cpu, "No se pudo crear los servidores de escucha");

        return;
    }

    pthread_t hilo_cpu_ds;
    pthread_t hilo_cpu_it;

    t_procesar_server* args_ds = malloc(sizeof(t_procesar_server));
    args_ds->logger = logger_cpu;
    args_ds->socket_server = md_cpu_ds;
    args_ds->server_name = "DISPATCH";

    t_procesar_server* args_it = malloc(sizeof(t_procesar_server));
    args_it->logger = logger_cpu;
    args_it->socket_server = md_cpu_it;
    args_it->server_name = "INTERRUPT";

    pthread_create(&hilo_cpu_ds, NULL, (void*) server_escuchar, (void*) args_ds); //Se guarda la info que tenemos antes en el struct
    pthread_create(&hilo_cpu_it, NULL, (void*) server_escuchar, (void*) args_it);

    pthread_join(hilo_cpu_ds, NULL);
    pthread_join(hilo_cpu_it, NULL);
}


void server_escuchar(void* args) {

    t_procesar_server* args_hilo = (t_procesar_server*) args;
    t_log* logger_server = args_hilo->logger;
    char* server_name = args_hilo->server_name;
    int socket_server = args_hilo->socket_server;

    while (1)
    {
        int socket_cliente = esperar_cliente(logger_server, server_name, socket_server);

        if(socket_cliente != -1) {
            atender_conexion(logger_server, server_name, socket_cliente);
        }
    }

}

void cerrar_programa(t_log* logger) {
    log_destroy(logger);
}