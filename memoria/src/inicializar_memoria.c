#include "inicializar_memoria.h"


t_config_memoria* config_memoria; // Variable global
//SEPARAR EN 2 
//un archivo config y otro conexiones 

int cargar_configuraciones_memoria(t_config_memoria* config_memoria) { //(char *path_config_memoria
    t_config* config = config_create("memoria.config");

    if(!config) {
        log_error(logger, "No se pudo cargar la configuracion de memoria");

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
        log_error(logger, "No se pudo cargar la configuracion de la memoria");

        return -1;
    }

    
    config_memoria->puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    config_memoria->tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
    config_memoria->tam_pagina = config_get_int_value(config, "TAM_PAGINA");
    copiar_valor(&config_memoria->path_instrucciones, config_get_string_value(config, "PATH_INSTRUCCIONES"));
    config_memoria->retardo_respuesta = config_get_int_value(config, "RETARDO_RESPUESTA");

    log_info(logger, "Configuraciones cargadas correctamente");
    config_destroy(config);

    return 1;
}

int crear_servidores(t_config_memoria* config_memoria, int *md_generico) {
    char* puerto_memoria = string_itoa(config_memoria->puerto_escucha); // Convierte un int a una cadena de char
    //Linux tarda
    *md_generico = iniciar_servidor("MEMORIA", "127.0.0.1", puerto_memoria);
    
    free(puerto_memoria);
    return (*md_generico != 0) ? 1 : -1;
}

void iniciar_modulo(t_config_memoria* config_memoria) {
    int md_generico = 0;
    char* server_name;

    if(crear_servidores(config_memoria, &md_generico) != 1) {
        log_error(logger, "No se pudo crear los servidores de escucha");
        return;
    }
    
    while (1)
    {
        
        int socket_cliente = esperar_cliente(server_name, md_generico);
        if(socket_cliente != -1) 
        {
          
			pthread_t hilo_memora;
			t_procesar_conexion *args_hilo = crear_procesar_conexion(server_name, socket_cliente);
            

            pthread_create(&hilo_memora, NULL, escuchar_peticiones, (void *) args_hilo);
            pthread_detach(hilo_memora);
        }
        
    }
    return md_generico;

    cerrar_programa(config_memoria, md_generico);
}

void cerrar_programa(t_config_memoria *config_memoria, int socket_server){
    config_destroy(config_memoria);
    close(socket_server);
}