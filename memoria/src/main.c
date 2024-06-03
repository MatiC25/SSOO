#include "main.h"


// void inicializar_config(t_config_memoria* config_memoria){
//     config_memoria = malloc(sizeof(t_config_memoria));

//     config_memoria->puerto_escucha = 0;
//     config_memoria->tam_memoria = 0;
//     config_memoria->tam_pagina = 0;
//     config_memoria->path_instrucciones = NULL;
//     config_memoria->retardo_respuesta = 0;
// }

int main()
{
    t_config_memoria* config_memoria = malloc(sizeof(t_config_memoria));
    
    config_memoria->puerto_escucha = 0;
    config_memoria->tam_memoria = 0;
    config_memoria->tam_pagina = 0;
    config_memoria->path_instrucciones = NULL;
    config_memoria->retardo_respuesta = 0;

    //Creando logger
    logger =  log_create("memoria.log","Memoria", 1, LOG_LEVEL_INFO);

    if (logger == NULL)
	{
		perror("No se puedo encontrar el archivo");
		return EXIT_FAILURE;
	}

    //inicializar_config(config_memoria); // Inicializo la variable global config_memoria! -> No se si es la mejor forma de hacerlo!
   
    //Inicializamos conexiones
    if (cargar_configuraciones_memoria(config_memoria) != 1)
    { // Generar conexiones, no va a mantener la conexion, sino que va a crear la conexion y la va a cerrar!
        log_error(logger, "Cargar las configuraciones");

        return EXIT_FAILURE;
    }
    
    //abrimos el servidor
    iniciar_modulo(config_memoria);

    return 0;
}