#include "int_entradasalida.h"

t_log*logge;

bool generar_conexiones(t_log *logger, t_config_k *config_entradasalida, int *md_memoria, int *md_kernel)
{
  char *ip_memoria = config_entradasalida->ip_memoria;
  char *puerto_memoria = string_itoa(config_entradasalida->puerto_memoria);

  char *ip_kernel = config_entradasalida->ip_kernel;
  char *puerto_kernel = string_itoa(config_entradasalida->puerto_kernel);

  //*md_memoria = crear_conexion(logger, "MEmoria", ip_memoria, puerto_memoria);
  *md_kernel = crear_conexion(logger, "kernel", ip_kernel, puerto_kernel);
  

  return /**md_memoria != 0 && */*md_kernel != 0; // Aca pregunto por el nuevo valor!
}

bool cargar_configuraciones(t_config_k *config_entradasalida, t_log *logger)
{
  t_config *config = config_create("entradasalida.config");

  if (config == NULL)
  {
    log_info(logger, "No se pudo abrir el archivo de configuraciones!");

    return false;
  }

  char *configuraciones[] = {
      "TIPO_INTERFAZ"
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

  copiar_valor(&config_entradasalida->tipo_interfaz, config_get_string_value(config, "TIPO_INTERFAZ"));
  copiar_valor(&config_entradasalida->ip_kernel, config_get_string_value(config, "IP_KERNEL"));
  copiar_valor(&config_entradasalida->ip_memoria, config_get_string_value(config, "IP_MEMORIA"));
  copiar_valor(&config_entradasalida->path_base_dialfs, config_get_string_value(config, "PATH_BASE_DIALFS"));

  config_entradasalida->tiempo_unidad_trabajo = config_get_int_value(config, "TIEMPO_UNIDAD_TRABAJO");
  config_entradasalida->puerto_kernel = config_get_int_value(config, "PUERTO_KERNEL");
  config_entradasalida->puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
  config_entradasalida->block_size = config_get_int_value(config, "BLOCK_SIZE");
  config_entradasalida->block_count = config_get_int_value(config, "BLOCK_COUNT");


  log_info(logger, "Se pudieron cargar todas las configuraciones necesarias!");
  config_destroy(config);
  

  return true;
}

void cerrar_programa(t_log *logger)
{
  log_destroy(logger);
}

void borrar_conexiones(int md_memoria, int md_kernel){
  liberar_conexion(md_memoria);
  liberar_conexion(md_kernel);
}
