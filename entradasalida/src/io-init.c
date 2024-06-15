#include "io-init.h"


void inicializar_interfaz(char *name_interfaz, char *config_path) {
    t_interfaz *interfaz = malloc(sizeof(t_interfaz));

    if (interfaz == NULL) {
        log_error(logger, "Error al asignar memoria para la interfaz");
        exit(EXIT_FAILURE);
    }
    cargar_configuraciones(config_path);
    //interfaz->config = 
    
    
}

void cargar_configuraciones(char *config_path) {
    t_config *config = config_create(config_path);

    if (config == NULL) {
        log_error(logger, "No se pudo cargar el archivo de configuración");
        exit(EXIT_FAILURE);
    }

    get_tipo_interfaz_with_config(config);

    inicializar_config_io();

    if (config_io == NULL) {
        log_error(logger, "Error al asignar memoria para config_io");
        config_destroy(config);
        exit(EXIT_FAILURE);
    }

    switch (interfaz->tipo) {
        case GENERICA:
            cargar_configuraciones_generica(config);
            break;
        case STDIN:
        case STDOUT:
            cargar_configuraciones_std(interfaz->tipo, config);
            break;
        case DIALFS:
            cargar_configuraciones_dialfs(config);
            break;
        default:
            log_error(logger, "Tipo de interfaz desconocido");
            config_destroy(config);
            //free(config_io);
            exit(EXIT_FAILURE);
    }

    config_destroy(config);

    
}

void cargar_configuraciones_generica(t_config *config) {
    char *configuraciones[] = {
        "TIEMPO_UNIDAD_UNIDAD",
        "IP_KERNEL",
        "PUERTO_KERNEL",
        NULL
    };

    validar_configuraciones(config, configuraciones);
    configurar_valores_kernel(config_io, config);
    configurar_tiempo_unidad(config_io, config);
}

void cargar_configuraciones_std(tipo_interfaz tipo, t_config *config) {
    char *configuraciones[] = {
        "IP_KERNEL",
        "PUERTO_KERNEL",
        "IP_MEMORIA",
        "PUERTO_MEMORIA",
        NULL
    };

    validar_configuraciones(config, configuraciones);
    configurar_valores_kernel(config_io, config);
    configurar_valores_memoria(config_io, config);

    if (tipo == STDIN && !config_has_property(config, "TIEMPO_UNIDAD_UNIDAD")) {
        log_error(logger, "Falta la configuración TIEMPO_UNIDAD_UNIDAD en el archivo de configuración");
        exit(EXIT_FAILURE);
    }

    configurar_tiempo_unidad(config_io, config);
}

void cargar_configuraciones_dialfs(t_config *config) {
    char *configuraciones[] = {
        "TIEMPO_UNIDAD_TRABAJO",
        "IP_KERNEL",
        "PUERTO_KERNEL",
        "IP_MEMORIA",
        "PUERTO_MEMORIA",
        "PATH_BASE_DIALFS",
        "BLOCK_SIZE",
        "BLOCK_COUNT",
        NULL
    };

    validar_configuraciones(config, configuraciones);
    configurar_tiempo_unidad(config_io, config);
    configurar_valores_kernel(config_io, config);
    configurar_valores_memoria(config_io, config);
    configurar_valores_dialfs(config_io, config);
}
void get_tipo_interfaz_with_config(t_config *config) {
    char *tipo = config_get_string_value(config, "TIPO");
    interfaz->tipo = get_tipo_interfaz(tipo);
}


tipo_interfaz get_tipo_interfaz(char *tipo) {
    if (string_equals_ignore_case(tipo, "GENERICA")) {
        return GENERICA;
    } else if (string_equals_ignore_case(tipo, "STDIN")) {
        return STDIN;
    } else if (string_equals_ignore_case(tipo, "STDOUT")) {
        return STDOUT;
    } else if (string_equals_ignore_case(tipo, "DIALFS")) {
        return DIALFS;
    } else {
        log_error(logger, "Tipo de interfaz desconocido");
        exit(EXIT_FAILURE);
    }
}