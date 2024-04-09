#include "init_kernel.h"


t_log* logger;


bool generar_conexiones(t_log *logger, t_config_k *config_kernel, int *md_memoria, int *md_cpu_dt, int *md_cpu_it)
{
  char *ip_memoria = config_kernel->ip_memoria;
  char *puerto_memoria = string_itoa(config_kernel->puerto_memoria);

  char *ip_cpu = config_kernel->ip_cpu;
  char *puerto_cpu_dispatch = string_itoa(config_kernel->puerto_cpu_ds);
  char *puerto_cpu_interrupt = string_itoa(config_kernel->puerto_cpu_it);

  *md_cpu_dt = crear_conexion(logger, "CPU-DT", ip_cpu, puerto_cpu_dispatch);
  *md_cpu_it = crear_conexion(logger, "CPU-IT", ip_cpu, puerto_cpu_interrupt);
  *md_memoria = crear_conexion(logger, "MEMORIA", ip_memoria, puerto_memoria); // Valores leidos de archivo de configuracion!

  return *md_memoria != 0 && *md_cpu_dt != 0 && *md_cpu_it != 0; // Aca pregunto por el nuevo valor!
}

bool cargar_configuraciones(t_config_k *config_kernel, t_log *logger)
{
  t_config *config = config_create("kernel.config");

  if (config == NULL)
  {
    log_info(logger, "No se pudo abrir el archivo de configuraciones!");

    return false;
  }

  char *configuraciones[] = {
      "PUERTO_ESCUCHA"
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

//para despues
/*
  if (!tiene_todas_las_configuraciones(config, configuraciones))
  {
    log_info(logger, "No se encontraron todas las configuraciones necesarias!");

    return false;
  }
*/
  copiar_valor(&config_kernel->puerto_escucha, config_get_string_value(config, "PUERTO_ESCUCHA"));
  copiar_valor(&config_kernel->ip_memoria, config_get_string_value(config, "IP_MEMORIA"));
  copiar_valor(&config_kernel->ip_cpu, config_get_string_value(config, "IP_CPU"));
  copiar_valor(&config_kernel->algoritmo_planificacion, config_get_string_value(config, "ALGORITMO_PLANIFICACION"));

  
  config_kernel->puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
  config_kernel->puerto_cpu_ds = config_get_int_value(config, "PUERTO_CPU_DISPATCH");
  config_kernel->puerto_cpu_it = config_get_int_value(config, "PUERTO_CPU_INTERRUPT");

//Para despues
/*
  if (!tiene_algun_algoritmo_de_planificacion(config_kernel->algoritmo_planificacion))
  {
    log_info(logger, "El algoritmo de planificacion no es valido!");

    return false;
  }
*/

//Crear conexiones no se puede todavia
  config_kernel->quantum = config_get_int_value(config, "QUANTUM");
/*
  crear_vector_dinamico_char(&config_kernel->recursos, config_get_array_value(config, "RECURSOS"));
  crear_vector_dinamico_int(&config_kernel->inst_recursos, config_get_array_value(config, "INSTANCIAS_RECURSOS"));

  log_info(logger, "Se pudieron cargar todas las configuraciones necesarias!");
  
  config_destroy(config);
  */

  return true;
}

void cerrar_programa(t_log *logger)
{
  log_destroy(logger);
}

void borrar_conexiones(int md_cpu_dt,int md_cpu_it, int md_memoria){
  liberar_conexion(md_cpu_dt);
  liberar_conexion(md_cpu_it);
  liberar_conexion(md_memoria);
}

//Servidor



int crear_servidor(t_log *logger, const char *name, char *ip, char *puerto){
  	
	int server_fd = iniciar_servidor(logger,"Kernel",NULL,puerto); //IP generica
	log_info(logger, "Servidor listo para recibir al cliente");
	int cliente_fd = esperar_cliente(logger,"Kernel",server_fd);

	t_list* lista;
	while (1) {
		int cod_op = recibir_operacion(cliente_fd);
		switch (cod_op) {
		case MENSAJE:
			recibir_mensaje(cliente_fd);
			break;
		case PAQUETE:
			lista = recibir_paquete(cliente_fd);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*)iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			break;
		}
	}
	return EXIT_SUCCESS;

  
}


void iterator(char* value) {
	log_info(logger,"%s", value);
}
