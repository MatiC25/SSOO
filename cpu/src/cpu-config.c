#include "cpu-config.h"

t_config_cpu* inicializar_config(void) {
    t_config_cpu *config_cpu = malloc(sizeof(t_config_cpu));

    config_cpu -> IP_MEMORIA = NULL;
    config_cpu -> PUERTO_MEMORIA = 0;
    config_cpu -> PUERTO_ESCUCHA_DISPATCH = 0;
    config_cpu -> PUERTO_ESCUCHA_INTERRUPT = 0;
    config_cpu -> CANTIDAD_ENTRADAS_TLB = 0;
    config_cpu -> ALGORITMO_TLB = NULL;
    config_cpu -> SOCKET_MEMORIA = -1;
    config_cpu -> SOCKET_DISPATCH = -1;
    config_cpu -> SOCKET_KERNEL = -1;

    return config_cpu;
}

void cargar_configuraciones(t_config_cpu* config_cpu) {
    t_config* config = config_create("./cpu.config");//path_config

    if(config == NULL) {
        log_error(logger, "No se pudo cargar la configuracion del filesystem");

        exit(-1);
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

    }
    
    copiar_valor(&config_cpu->IP_MEMORIA, config_get_string_value(config, "IP_MEMORIA")); 
    config_cpu->PUERTO_MEMORIA = config_get_int_value(config, "PUERTO_MEMORIA");
    config_cpu->PUERTO_ESCUCHA_DISPATCH = config_get_int_value(config, "PUERTO_ESCUCHA_DISPATCH");
    config_cpu->PUERTO_ESCUCHA_INTERRUPT = config_get_int_value(config, "PUERTO_ESCUCHA_INTERRUPT");
    copiar_valor(&config_cpu->CANTIDAD_ENTRADAS_TLB, config_get_string_value(config, "CANTIDAD_ENTRADAS_TLB")); 
    config_cpu->ALGORITMO_TLB = config_get_string_value(config, "ALGORITMO_TLB");


    log_info(logger, "Configuraciones cargadas correctamente");
    config_destroy(config);
}