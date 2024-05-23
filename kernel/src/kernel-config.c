#include "kernel-config.h"

t_config_kernel* inicializar_config_kernel() {
	t_config_kernel* config_kernel = malloc(sizeof(t_config_kernel));

	config_kernel->PUERTO_ESCUCHA = 0;
	config_kernel->IP_MEMORIA = NULL;
	config_kernel->PUERTO_MEMORIA = 0;
	config_kernel->IP_CPU = NULL;
	config_kernel->PUERTO_CPU_DS = 0;
	config_kernel->PUERTO_CPU_IT = 0;
	config_kernel->ALGORITMO_PLANIFICACION = NULL;
	config_kernel->QUANTUM = 0;
	config_kernel->RECURSOS = NULL;
	config_kernel->INST_RECURSOS = NULL;
	config_kernel->GRADO_MULTIP = 0;

	return config_kernel;
}

void cargar_configuraciones(t_config_kernel* config_kernel) {
	t_config *config = config_create("kernel.config");

	if (config == NULL) {
		log_info(logger, "No se pudo abrir el archivo de configuraciones!");

		exit(-1);
	}

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

	if (!tiene_todas_las_configuraciones(config, configuraciones)) {
		log_info(logger, "No se encontraron todas las configuraciones necesarias!");

		exit(-1);
	}

	copiar_valor(&config_kernel->IP_MEMORIA, config_get_string_value(config, "IP_MEMORIA"));
	copiar_valor(&config_kernel->IP_CPU, config_get_string_value(config, "IP_CPU"));
	copiar_valor(&config_kernel->ALGORITMO_PLANIFICACION, config_get_string_value(config, "ALGORITMO_PLANIFICACION"));
	crear_vector_dinamico_char(&config_kernel->RECURSOS, config_get_array_value(config, "RECURSOS"));
	crear_vector_dinamico_int(&config_kernel->INST_RECURSOS, config_get_array_value(config, "INSTANCIAS_RECURSOS"));

	config_kernel->PUERTO_ESCUCHA = config_get_int_value(config, "PUERTO_ESCUCHA");
	config_kernel->PUERTO_MEMORIA = config_get_int_value(config, "PUERTO_MEMORIA");
	config_kernel->PUERTO_CPU_DS = config_get_int_value(config, "PUERTO_CPU_DISPATCH");
	config_kernel->PUERTO_CPU_IT = config_get_int_value(config, "PUERTO_CPU_INTERRUPT");
	config_kernel->QUANTUM = config_get_int_value(config, "QUANTUM");
	config_kernel->GRADO_MULTIP = config_get_int_value(config, "GRADO_MULTIPROGRAMACION");

	log_info(logger, "Se pudieron cargar todas las configuraciones necesarias!");
	config_destroy(config);
}