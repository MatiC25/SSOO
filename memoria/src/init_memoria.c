#include "init_memoria.h"

int cargar_configuraciones(t_config_memoria* config_memoria, t_log* logger_memoria) {
    t_config* config = config_create("memoria.config");

    if(config_memoria == NULL) {
        log_error(logger_memoria, "No se pudo cargar la configuracion del filesystem");

        return -1;
    }

    char* configuraciones[] = {
        "PUERTO_ESCUCHA",
        "TAM_MEMORIA",
        "TAM_PAGINA",
        "PATH_INSTRUCCIONES",
        "RETARDO_RESPUESTA",
        NULL
    };

    if(!tiene_todas_las_configuraciones(config, configuraciones)) {
        log_error(logger_memoria, "No se pudo cargar la configuracion de la memoria");

        return -1;
    }
    

    config_memoria->puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    config_memoria->tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
    config_memoria->tam_pagina = config_get_int_value(config, "TAM_PAGINA");
    copiar_valor(&config_memoria->path_instrucciones, config_get_string_value(config, "PATH_INSTRUCCIONES"));
    config_memoria->retardo_respuesta = config_get_int_value(config, "RETARDO_RESPUESTA");

    log_info(logger_memoria, "Configuraciones cargadas correctamente");
    config_destroy(config);

    return 1;
}

int crear_servidores(t_log* logger_memoria, t_config_memoria* config_memoria, int* md_kernel, int* md_EntradaySalida, int* md_cpu) {
    char* puerto_memoria = string_itoa(config_memoria->puerto_escucha); // Convierte un int a una cadena de char
    char* ip_memoria = NULL;

    *md_kernel = iniciar_servidor(logger_memoria, "KERNEL", ip_memoria, puerto_memoria); // Guarda ID del socket
    *md_EntradaySalida = iniciar_servidor(logger_memoria, "I/O", ip_memoria, puerto_memoria); 
    *md_cpu = iniciar_servidor(logger_memoria, "CPU", ip_memoria, puerto_memoria); 

    return (*md_kernel != 0 && *md_cpu != 0 && *md_EntradaySalida != 0) ? 1 : -1;
}

void iniciar_modulo(t_log* logger_memoria, t_config_memoria* config_memoria) {
    int md_kernel = 0;
    int md_EntradaySalida = 0;
    int md_cpu = 0;

    if( crear_servidores(logger_memoria, config_memoria, &md_kernel, &md_EntradaySalida, &md_cpu) != 1) {
        log_error(logger_memoria, "No se pudo crear los servidores de escucha");

        return;
    }





    //Creamos un hilo por cada proceso para administrar los requerimientos de concurrencia
    //Tambien pensanmos en agregar hilos para la administración de recursos
    pthread_t hilo_kernel;
    pthread_t hilo_cpu;
    pthread_t hilo_entradaysalida;

    t_procesar_server* args_k = malloc(sizeof(t_procesar_server));
    args_k->logger_memoria = logger_memoria;
    args_k->socket_server = md_kernel;
    args_k->server_name = "KERNEL";

    t_procesar_server* args_cpu = malloc(sizeof(t_procesar_server));
    args_cpu->logger_memoria = logger_memoria;
    args_cpu->socket_server = md_cpu;
    args_cpu->server_name = "CPU";

    t_procesar_server* args_i_o = malloc(sizeof(t_procesar_server));
    args_i_o->logger_memoria = logger_memoria;
    args_i_o->socket_server = md_EntradaySalida;
    args_i_o->server_name = "I/O";

    pthread_create(&hilo_kernel, NULL, (void*) server_escuchar, (void*) args_k); //Se guarda la info que tenemos antes en el struct
    pthread_create(&hilo_cpu, NULL, (void*) server_escuchar, (void*) args_cpu);
    pthread_create(&hilo_entradaysalida, NULL, (void*) server_escuchar, (void*) args_i_o);

    pthread_join(hilo_kernel, NULL);
    pthread_join(hilo_cpu, NULL);
    pthread_join(hilo_entradaysalida, NULL);
}

void cerrar_programa(t_log* logger_memoria) {
    log_destroy(logger_memoria);
}

void server_escuchar(void* args) {
    t_procesar_server* args_hilo = (t_procesar_server*) args;
    t_log* logger_server = args_hilo->logger_memoria;
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















