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
    
    copiar_valor(&config_memoria->path_instrucciones, config_get_string_value(config, "PATH_INSTRUCCIONES"));

    config_memoria->puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    config_memoria->tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
    config_memoria->tam_pagina = config_get_int_value(config, "TAM_PAGINA");
    config_memoria->retardo_respuesta = config_get_int_value(config, "RETARDO_RESPUESTA");

    log_info(logger_memoria, "Configuraciones cargadas correctamente");
   config_destroy(config);

    return 1;
}

int crear_servidores(t_log* logger_memoria, t_config_memoria* config_memoria, int* md_generico) {
    char* puerto_memoria = string_itoa(config_memoria->puerto_escucha); // Convierte un int a una cadena de char
    
    *md_generico = iniciar_servidor(logger_memoria, "Memoria" , "127.0.0.1", puerto_memoria);

    return (*md_generico != 0) ? 1 : -1;
}

void iniciar_modulo(t_log* logger_memoria, t_config_memoria* config_memoria) {
    int* md_generico = 0;

    if(crear_servidores(logger_memoria, config_memoria, &md_generico) != 1) {
        log_error(logger_memoria, "No se pudo crear los servidores de escucha");

        return;
    }

    server_escuchar_con_hilos(logger_memoria, "Memoria", md_generico);
}

void cerrar_programa(t_log *logger_memoria, t_config_memoria *config_memoria)
{
    log_destroy(logger_memoria);
    config_destroy(config_memoria);
}