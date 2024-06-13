#include "config_memoria.h"


t_config_memoria* inicializar_config_memoria(void){

    t_config_memoria* config_memoria = malloc(sizeof(t_config_memoria));
    config_memoria->puerto_escucha = 0;
    config_memoria->tam_memoria = 0;
    config_memoria->tam_pagina = 0;
    config_memoria->path_instrucciones = 0;
    config_memoria->retardo_respuesta = 0;
    return config_memoria;
}


int cargar_configuraciones_memoria(t_config_memoria* config_memoria) { //(char *path_config_memoria
    t_config* config = config_create("memoria.config");

    if(config_memoria == NULL) {
        log_error(logger, "No se pudo cargar la configuracion del filesystem");

         return -1;
    }

    char* configuraciones[] = {
        "PUERTO_ESCUCHA"
        "TAM_MEMORIA"
        "TAM_PAGINA"
        "PATH_INSTRUCCIONES"
        "RETARDO_RESPUESTA"
    };

    if(!tiene_todas_las_configuraciones(config, configuraciones)) {
        log_error(logger, "No se pudo cargar la configuracion de la memoria");

         return 1;
    }
    
    copiar_valor(&config_memoria->path_instrucciones, config_get_string_value(config, "PATH_INSTRUCCIONES"));

    config_memoria->puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    config_memoria->tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
    config_memoria->tam_pagina = config_get_int_value(config, "TAM_PAGINA");
    config_memoria->retardo_respuesta = config_get_int_value(config, "RETARDO_RESPUESTA");

    log_info(logger, "Configuraciones cargadas correctamente");
    config_destroy(config);
}