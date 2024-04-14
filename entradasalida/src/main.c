#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "int_entradasalida.h"

t_config_k *config_entradasalida; // La declaro como variable global, y la vez instanciar aca!

void inicializar_config(void)
{
    config_entradasalida = malloc(sizeof(t_config_k));
    config_entradasalida->tipo_interfaz = NULL;
    config_entradasalida->ip_kernel = NULL;
    config_entradasalida->ip_memoria = NULL;
    config_entradasalida->path_base_dialfs = NULL;
}

int main()
{

    //Creando logger
    t_log* logger_entradasalida = log_create("entradasalida.log","I/O",1, LOG_LEVEL_INFO);
    if ( logger_entradasalida == NULL)
	{
		perror("No se puedo encontrar el archivo");
		return EXIT_FAILURE;
	}
    log_info(logger_entradasalida,"Hola soy un log");

    inicializar_config(); // Inicializo la variable global config_entradasalida!

    int md_memoria = 0, md_kernel = 0;
<<<<<<< HEAD
    if (cargar_configuraciones(config_entradasalida, logger_entradasalida) != 1 || generar_conexiones(logger_entradasalida, config_entradasalida, &md_memoria, &md_kernel) != 1 )
=======
    if (cargar_configuraciones(config_entradasalida, logger_entradasalida) != 1 && generar_conexiones(logger_entradasalida, config_entradasalida, &md_memoria, &md_kernel) != 1 )
>>>>>>> 3daa545628a9ab15344bec7b627175417907172e
    { // Generar conexiones, no va a mantener la conexion, sino que va a crear la conexion y la va a cerrar!
        log_error(logger_entradasalida, "No se pudieron generar las conexiones");

        return 1;
    }


  
   return 0;
}