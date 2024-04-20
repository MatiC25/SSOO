#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "int_entradasalida.h"

t_config_io *config_entradasalida; // La declaro como variable global, y la vez instanciar aca!

void inicializar_config(void)
{
    config_entradasalida = malloc(sizeof(t_config_io));
    config_entradasalida->tipo_interfaz = NULL;
    config_entradasalida->ip_kernel = NULL;
    config_entradasalida->ip_memoria = NULL;
    config_entradasalida->path_base_dialfs = NULL;
}

int main()
{
    //Creando logger
    logger = log_create("entradasalida.log","I/O",1, LOG_LEVEL_INFO);
    if (logger == NULL)
	{
		perror("No se puedo encontrar el archivo");
		return EXIT_FAILURE;
	}
    log_info(logger,"Hola soy un log");

    inicializar_config(); // Inicializo la variable global config_entradasalida!

    int md_memoria = 0, md_kernel = 0;
    
    if (cargar_configuraciones(config_entradasalida) != 1 || generar_conexiones(config_entradasalida, &md_memoria, &md_kernel) != 1 )
    { // Generar conexiones, no va a mantener la conexion, sino que va a crear la conexion y la va a cerrar!
        log_error(logger, "No se pudieron generar las conexiones");

        return 1;
    }

    char *valor = "hola";

    enviar_mensaje(valor, md_memoria);
    enviar_mensaje(valor, md_kernel);

    
    //Evios de paquetes
    paquete(md_memoria);
    paquete(md_kernel);
   

    cerrar_programa(config_entradasalida, md_memoria, md_kernel);
  
   return 0;
}