#include "init_kernel.h"


t_log* logger_kernel;


int generar_conexiones(t_log *logger_kernel, t_config_k *config_kernel, int *md_memoria, int *md_cpu_dt, int *md_cpu_it)
{
  char *ip_memoria = config_kernel->ip_memoria;
  char *puerto_memoria = string_itoa(config_kernel->puerto_memoria);

  char *ip_cpu = config_kernel->ip_cpu;
  char *puerto_cpu_dispatch = string_itoa(config_kernel->puerto_cpu_ds);
  char *puerto_cpu_interrupt = string_itoa(config_kernel->puerto_cpu_it);

  *md_cpu_dt = crear_conexion(logger_kernel, "CPU-DT", ip_cpu, puerto_cpu_dispatch);
  *md_cpu_it = crear_conexion(logger_kernel, "CPU-IT", ip_cpu, puerto_cpu_interrupt);
  *md_memoria = crear_conexion(logger_kernel, "MEMORIA", ip_memoria, puerto_memoria); // Valores leidos de archivo de configuracion!

//paquete(md_cpu_dt);
//paquete(md_cpu_it);

  return (*md_cpu_dt != 0 && *md_cpu_it != 0 && *md_memoria != 0) ? 1 : -1; //Aca pregunto por el nuevo valor!
}

int cargar_configuraciones(t_config_k *config_kernel, t_log *logger_kernel)
{
  t_config *config = config_create("kernel.config");

  if (config == NULL)
  {
    log_info(logger_kernel, "No se pudo abrir el archivo de configuraciones!");

    return -1;
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

//Para despues
/*
  if (!tiene_todas_las_configuraciones(config, configuraciones))
  {
    log_info(logger_kernel, "No se encontraron todas las configuraciones necesarias!");

    return -1 ;
  }
*/
  //copiar_valor(&config_kernel->puerto_escucha, config_get_string_value(config, "PUERTO_ESCUCHA"));
  copiar_valor(&config_kernel->ip_memoria, config_get_string_value(config, "IP_MEMORIA"));
  copiar_valor(&config_kernel->ip_cpu, config_get_string_value(config, "IP_CPU"));
  copiar_valor(&config_kernel->algoritmo_planificacion, config_get_string_value(config, "ALGORITMO_PLANIFICACION"));

  config_kernel->puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
  config_kernel->puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
  config_kernel->puerto_cpu_ds = config_get_int_value(config, "PUERTO_CPU_DISPATCH");
  config_kernel->puerto_cpu_it = config_get_int_value(config, "PUERTO_CPU_INTERRUPT");

//Para despues
/*
  if (!tiene_algun_algoritmo_de_planificacion(config_kernel->algoritmo_planificacion))
  {
    log_info(logger_kernel, "El algoritmo de planificacion no es valido!");

    return false;
  }
*/

//Crear conexiones no se puede todavia
  config_kernel->quantum = config_get_int_value(config, "QUANTUM");
/*
  crear_vector_dinamico_char(&config_kernel->recursos, config_get_array_value(config, "RECURSOS"));
  crear_vector_dinamico_int(&config_kernel->inst_recursos, config_get_array_value(config, "INSTANCIAS_RECURSOS"));

  log_info(logger_kernel, "Se pudieron cargar todas las configuraciones necesarias!");
  
  config_destroy(config);
  */

  return 1;
}





int crear_servidor(t_log* logger_kernel, t_config_k* config_kernel, int * md_EntradaySalida) {
    char* puerto_entradasalida = string_itoa(config_kernel->puerto_escucha); // Convierte un int a una cadena de char
    char* ip_memoria = NULL;  //Ip generica

    *md_EntradaySalida = iniciar_servidor(logger_kernel, "I/O", ip_memoria, puerto_entradasalida); // Guarda ID del socket
    

    return (md_EntradaySalida != 0) ? 1 : -1 ;
}


void server_escuchar(void* args) {
    t_procesar_server* args_hilo = (t_procesar_server*) args;
    t_log* logger_server = args_hilo->logger_kernel;
    char* server_name = args_hilo->server_name;
    int socket_server = args_hilo->socket_server;

    while (1)
    {
        int socket_cliente = esperar_cliente(logger_server, server_name, socket_server);

        if(socket_cliente != -1) {
            atender_conexion(logger_server, server_name, socket_cliente);
        }
      }
    }


void iniciar_modulo(t_log* logger_kernel, t_config_k* config_kernel) {
int md_EntradaySalida = 0;

      if(crear_servidor(logger_kernel, config_kernel, & md_EntradaySalida) != 1) {
        log_error(logger_kernel, "No se pudo crear los servidores de escucha");

        return ;
    }

  
    pthread_t hilo_enetradaysalida;
    
    t_procesar_server* args_ds = malloc(sizeof(t_procesar_server));
    args_ds->logger_kernel = logger_kernel;
    args_ds->socket_server = md_EntradaySalida;
    args_ds->server_name = "I/O";

    pthread_create(&hilo_enetradaysalida, NULL, (void*) server_escuchar, (void*) args_ds); //Se guarda la info que tenemos antes en el struct
    pthread_join(hilo_enetradaysalida, NULL);
    
  
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido;
	t_paquete* paquete = crear_paquete();

	// Leemos y esta vez agregamos las lineas al paquete
	leido = readline("> ");
	while(strcmp(leido, "") != 0)
	{	
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		free(leido);
		leido = readline("> ");
	}

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	free(leido);
	enviar_paquete(paquete, conexion);
	eliminar_paquete(paquete);
}


void cerrar_programa(t_log *logger_kernel)
{
  log_destroy(logger_kernel);
}

void borrar_conexiones(int md_cpu_dt,int md_cpu_it, int md_memoria){
  liberar_conexion(md_cpu_dt);
  liberar_conexion(md_cpu_it);
  liberar_conexion(md_memoria);
}
