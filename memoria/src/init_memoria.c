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

int crear_servidores(t_log* logger_memoria, t_config_memoria* config_memoria, int* md_generico) {
    char* puerto_memoria = string_itoa(config_memoria->puerto_escucha); // Convierte un int a una cadena de char
    char* ip_memoria = NULL;

    md_generico = iniciar_servidor(logger_memoria, puerto_memoria, ip_memoria);

    return md_generioco != 0;
}

void iniciar_modulo(t_log* logger_memoria, t_config_memoria* config_memoria) {
    int md_generico = 0;

    if(crear_servidores(logger_memoria, config_memoria, md_generioco) != 1) {
        log_error(logger_memoria, "No se pudo crear los servidores de escucha");

        return;
    }

    server_escuchar(logger_memoria, "Memoria", md_generico);
    //Creamos un hilo por cada proceso para administrar los requerimientos de concurrencia
    //Tambien pensanmos en agregar hilos para la administración de recursos
}

void cerrar_programa(t_log* logger_memoria) {
    log_destroy(logger_memoria);
}

void server_escuchar(t_log* logger_memoria, const char* server_name, int socket_server) {
    // t_procesar_server* args_hilo = (t_procesar_server*) args;
    // t_log* logger_server = args_hilo->logger_memoria;
    // char* server_name = args_hilo->server_name;
    // int socket_server = args_hilo->socket_server;

    while (1)
    {
        int socket_cliente = esperar_cliente(logger_server, server_name, socket_server);

        if(socket_cliente != -1) {
            pthread_t hilo;
            t_procesar_cliente* args_hilo = malloc(sizeof(t_procesar_cliente));
            args_hilo->logger_server = logger_server;
            args_hilo->server_name = server_name;
            args_hilo->socket_cliente = socket_cliente;

            pthread_create(&hilo, NULL, (void*) atender_conexion, args_hilo);
            pthread_detach(hilo);
        }
    }

}

void atender_conexiones_memoria(void *args)
{
	t_procesar_cliente *args_hilo = (t_procesar_cliente *)args;
	t_log *logger = args_hilo->logger_memoria;
	char *server_name = args_hilo->server_name;
	int cliente_socket = args_hilo->socket_cliente;

	op_code cop;

	// while (1)
	// {
		
	// }

	log_warning(logger, "El cliente se desconecto de %s server", server_name);

	return;
}













