#include "init_kernel.h"


int generar_conexiones(t_config_k *config_kernel, int *md_memoria, int *md_cpu_dt, int *md_cpu_it)
{
  char *ip_memoria = config_kernel->ip_memoria;
  char *puerto_memoria = string_itoa(config_kernel->puerto_memoria);

  char *ip_cpu = config_kernel->ip_cpu;
  char *puerto_cpu_dispatch = string_itoa(config_kernel->puerto_cpu_ds);
  char *puerto_cpu_interrupt = string_itoa(config_kernel->puerto_cpu_it);

  *md_cpu_dt = crear_conexion("CPU-DT", ip_cpu, puerto_cpu_dispatch);
  *md_cpu_it = crear_conexion("CPU-IT", ip_cpu, puerto_cpu_interrupt);
  *md_memoria = crear_conexion("MEMORIA", ip_memoria, puerto_memoria); // Valores leidos de archivo de configuracion!


  return (*md_cpu_dt != 0 && *md_cpu_it != 0 && *md_memoria != 0) ? 1 : -1; //Aca pregunto por el nuevo valor!
}

int cargar_configuraciones(t_config_k *config_kernel)
{
  t_config *config = config_create("kernel.config");

  if (config == NULL)
  {
    log_info(logger, "No se pudo abrir el archivo de configuraciones!");

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
    log_info(logger, "No se encontraron todas las configuraciones necesarias!");

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
    log_info(logger, "El algoritmo de planificacion no es valido!");

    return false;
  }
*/

//Crear conexiones no se puede todavia
  config_kernel->quantum = config_get_int_value(config, "QUANTUM");

  crear_vector_dinamico_char(&config_kernel->recursos, config_get_array_value(config, "RECURSOS"));
  crear_vector_dinamico_int(&config_kernel->inst_recursos, config_get_array_value(config, "INSTANCIAS_RECURSOS"));

  log_info(logger, "Se pudieron cargar todas las configuraciones necesarias!");
  

   
  config_destroy(config);
  

  return 1;
}


int crear_servidor(t_config_k* config_kernel, int * md_EntradaySalida) {
    char* puerto_entradasalida = string_itoa(config_kernel->puerto_escucha); // Convierte un int a una cadena de char
    //char* ip_memoria = NULL;  //Ip generica

    *md_EntradaySalida = iniciar_servidor("I/0", NULL, puerto_entradasalida);

    return (md_EntradaySalida != 0) ? 1 : -1 ; 
}



void iniciar_modulo(t_config_k* config_kernel) {
    int md_EntradaySalida = 0;

    if(crear_servidor(config_kernel, &md_EntradaySalida) != 1) 
    {
        log_error(logger, "No se pudo crear los servidores de escucha");

        return ;
    }
  
    pthread_t hilo_enetradaysalida;
    t_procesar_conexion* args_ds = crear_procesar_conexion("ENTRADAYSALIDA", md_EntradaySalida);

    pthread_create(&hilo_enetradaysalida, NULL, (void*) server_escuchar_sin_hilos, (void*) args_ds); //Se guarda la info que tenemos antes en el struct
    pthread_join(hilo_enetradaysalida, NULL);
}

void cerrar_programa(t_config_k *config_kernel, int md_memoria, int md_cpu_dt, int md_cpu_it)
{
  log_destroy(logger);
  destruir_configuracion_k(config_kernel);
  close(md_memoria);
  close(md_cpu_dt);
  close(md_cpu_it);
}

void destruir_configuracion_k(t_config_k *config_kernel){
  if (config_kernel == NULL) {
        return; // No hay nada que liberar
    }

    free(config_kernel->ip_memoria);//Liberar recursos internos
    free(config_kernel->ip_cpu);
    free(config_kernel->algoritmo_planificacion);
    if (config_kernel->recursos != NULL) {
        for (int i = 0; config_kernel->recursos[i] != NULL; i++) {
            free(config_kernel->recursos[i]);
        }
        free(config_kernel->recursos);
    }

    free(config_kernel); //Liberar la estructura principal
}