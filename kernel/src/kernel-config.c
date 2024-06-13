#include "kernel-config.h"

// Funciones para inicializar:

t_config_kernel* inicializar_config_kernel() {
    t_config_kernel* config_kernel = malloc(sizeof(t_config_kernel));

    config_kernel->PUERTO_ESCUCHA = 0;
    config_kernel->IP_MEMORIA = NULL;
    config_kernel->PUERTO_MEMORIA = 0;
    config_kernel->IP_CPU = NULL;
    config_kernel->SOCKET_DISPATCH = 0;
    config_kernel->SOCKET_INTERRUPT = 0;
    config_kernel->ALGORITMO_PLANIFICACION = NULL;
    config_kernel->QUANTUM = 0;
    config_kernel->RECURSOS = calloc(MAX_RECURSOS, sizeof(char*));
    config_kernel->INST_RECURSOS = calloc(MAX_RECURSOS, sizeof(int));
    config_kernel->GRADO_MULTIP = 0;
    config_kernel->PUERTO_CPU_DS = 0;
    config_kernel->PUERTO_CPU_IT = 0;
    config_kernel->PUERTO_KERNEL = 0;
    config_kernel->IP_KERNEL = NULL;

    return config_kernel;
}

t_config_kernel *cargar_config_kernel(char *path_config) {
	t_config_kernel *config_kernel = inicializar_config_kernel();
	cargar_configuraciones(config_kernel, path_config);

	return config_kernel;
}

// Funciones para cargar configuraciones:

void cargar_configuraciones(t_config_kernel* config_kernel, char *path_config) {
	t_config *config = config_create("kernel.config"); //path_config

	if (config == NULL) {
		log_info(logger, "No se pudo abrir el archivo de configuraciones!");

		exit(-1);

	char *configuraciones[] = {
		"PUERTO_ESCUCHA",
		"IP_MEMORIA",
		"PUERTO_MEMORIA",
		"IP_CPU",
		"PUERTO_CPU_DISPATCH",
		"PUERTO_CPU_INTERRUPT",
		"ALGORITMO_PLANIFICACION",
		"QUANTUM",
		"RECURSOS",
		"INSTANCIAS_RECURSOS",
		"GRADO_MULTIPROGRAMACION",
		NULL
	};

	if(!tiene_todas_las_configuraciones(config, configuraciones)){
		log_error(logger,"NO se pudieron cargar todas las configuraciones");
	}

	cargar_valores_de_memoria(config, config_kernel);
	cargar_valores_de_cpu(config, config_kernel);
	cargar_valores_de_planificacion(config, config_kernel);
	cargar_valores_de_recursos(config, config_kernel);
	cargar_valores_de_grado_multiprogramacion(config, config_kernel);

	info_config(config);
}
}

// Funciones para cargar valores:

void cargar_valores_de_memoria(t_config *config, t_config_kernel *config_kernel) {
	copiar_valor(&config_kernel->IP_MEMORIA, config_get_string_value(config, "IP_MEMORIA"));
	config_kernel->PUERTO_MEMORIA = config_get_int_value(config, "PUERTO_MEMORIA");
}

void cargar_valores_de_cpu(t_config *config, t_config_kernel *config_kernel) {
	copiar_valor(&config_kernel->IP_CPU, config_get_string_value(config, "IP_CPU"));
	config_kernel->PUERTO_CPU_DS = config_get_int_value(config, "PUERTO_CPU_DISPATCH");
	config_kernel->PUERTO_CPU_IT = config_get_int_value(config, "PUERTO_CPU_INTERRUPT");
}

void cargar_valores_de_planificacion(t_config *config, t_config_kernel *config_kernel) {
	copiar_valor(&config_kernel->ALGORITMO_PLANIFICACION, config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
	config_kernel->QUANTUM = config_get_int_value(config, "QUANTUM");
}

void cargar_valores_de_recursos(t_config* config, t_config_kernel* config_kernel) {
    char** recursos = config_get_array_value(config, "RECURSOS");
    char** instancias_str = config_get_array_value(config, "INSTANCIAS_RECURSOS");

    for (int i = 0; i < MAX_RECURSOS; i++) {
        if (recursos[i] != NULL && instancias_str[i] != NULL) {
            config_kernel->RECURSOS[i] = strdup(recursos[i]);
            config_kernel->INST_RECURSOS[i] = atoi(instancias_str[i]);
        } else {
            config_kernel->RECURSOS[i] = NULL;
            config_kernel->INST_RECURSOS[i] = 0;
        }
    }

    liberar_array(recursos);
    liberar_array(instancias_str);
}


void liberar_array(char** array) {
    for (int i = 0; array[i] != NULL; i++) {
        free(array[i]);
    }
    free(array);
}


void cargar_valores_de_grado_multiprogramacion(t_config *config, t_config_kernel *config_kernel) {
	config_kernel->GRADO_MULTIP = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");
}

// Funciones para extraer valores:

int get_socket_memoria() {
	return config_kernel->SOCKET_MEMORIA;
}

int get_socket_dispatch() {
	return config_kernel->SOCKET_DISPATCH;
}

int get_socket_interrupt() {
	return config_kernel->SOCKET_INTERRUPT;
}

void set_socket_memoria(int socket) {
	config_kernel->SOCKET_MEMORIA = socket;
}

void set_socket_dispatch(int socket) {
	config_kernel->SOCKET_DISPATCH = socket;
}

void set_socket_interrupt(int socket) {
	config_kernel->SOCKET_INTERRUPT = socket;
}