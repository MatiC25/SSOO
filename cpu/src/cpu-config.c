#include "cpu-config.h"

t_config_cpu* inicializar_config(char *path_config_cpu) {
    t_config_cpu *config_cpu = malloc(sizeof(t_config_cpu));

    config_cpu -> IP_MEMORIA = NULL;
    config_cpu -> PUERTO_MEMORIA = 0;
    config_cpu -> PUERTO_ESCUCHA_DISPATCH = 0;
    config_cpu -> PUERTO_ESCUCHA_INTERRUPT = 0;
    config_cpu -> CANTIDAD_ENTRADAS_TLB = 0;
    config_cpu -> ALGORITMO_TLB = NULL;
    config_cpu -> SOCKET_MEMORIA = -1;
    config_cpu -> SOCKET_DISPATCH = -1;

    return config_cpu;
}

void cargar_configuraciones(char* path_config_cpu) {
    t_config* config_cpu = config_create(path_config_cpu);

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

        exit(-1);
    }
    
    copiar_valor(&config_cpu->ip_memoria, config_get_string_value(config, "IP_MEMORIA"));

    config_cpu->puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
    config_cpu->puerto_escucha_dispatch = config_get_int_value(config, "PUERTO_ESCUCHA_DISPATCH");
    config_cpu->puerto_escucha_interrupt = config_get_int_value(config, "PUERTO_ESCUCHA_INTERRUPT");

    log_info(logger, "Configuraciones cargadas correctamente");
    config_destroy(config);
}