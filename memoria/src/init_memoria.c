#include "init_memoria.h"

char *path_proceso;
//SEPARAR EN 2 
//un archivo config y otro conexiones 

int cargar_configuraciones(t_config_memoria* config_memoria) { //(char *path_config_memoria
    t_config* config = config_create("memoria.config");

    if(config_memoria == NULL) {
        log_error(logger, "No se pudo cargar la configuracion del filesystem");

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
    
    copiar_valor(&config_memoria->path_instrucciones, config_get_string_value(config, "PATH_INSTRUCCIONES"));

    config_memoria->puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    config_memoria->tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
    config_memoria->tam_pagina = config_get_int_value(config, "TAM_PAGINA");
    config_memoria->retardo_respuesta = config_get_int_value(config, "RETARDO_RESPUESTA");

    log_info(logger, "Configuraciones cargadas correctamente");
    config_destroy(config);

    return 1;
}

int crear_servidores(t_config_memoria* config_memoria, int *md_generico) {
    char* puerto_memoria = string_itoa(config_memoria->puerto_escucha); // Convierte un int a una cadena de char
    //Linux tarda

    *md_generico = iniciar_servidor("Memoria" , "127.0.0.1", puerto_memoria);

    return (*md_generico != 0) ? 1 : -1;
}

void iniciar_modulo(t_config_memoria* config_memoria) {
    int md_generico = 0;

    if(crear_servidores(config_memoria, &md_generico) != 1) {
        log_error(logger, "No se pudo crear los servidores de escucha");

        return;
    }

    // signal(SIGINT, handler);
    server_escuchar_con_hilos("Memoria", md_generico);
    // cerrar_programa(logger_memoria, config_memoria, md_generico);
}

void cerrar_programa(t_config_memoria *config_memoria, int socket_server)
{
    log_destroy(logger);
    // Falta implementar una funcion que se haga cargo de elimiar el t_config de cada modulo :)
    close(socket_server);
}