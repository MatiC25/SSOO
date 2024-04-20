#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "init_memoria.h"

t_config_memoria* config_memoria;


void inicializar_config(void)
{
    config_memoria = malloc(sizeof(t_config_memoria));
    config_memoria->puerto_escucha = NULL;
    config_memoria->tam_memoria = NULL;
    config_memoria->tam_pagina = NULL;
    config_memoria->path_instrucciones = NULL;
    config_memoria->retardo_respuesta = NULL;
}

int main()
{
    //Creando logger
    logger =  log_create("memoria.log","Memoria",1, LOG_LEVEL_INFO);

    if ( logger == NULL)
	{
		perror("No se puedo encontrar el archivo");
		return EXIT_FAILURE;
	}

    inicializar_config(); // Inicializo la variable global config_memoria! -> No se si es la mejor forma de hacerlo!
   
    //Inicializamos conexiones
    if (cargar_configuraciones(config_memoria) != 1)
    { // Generar conexiones, no va a mantener la conexion, sino que va a crear la conexion y la va a cerrar!
        log_error(logger, "Cargar las configuraciones");

        return EXIT_FAILURE;
    }
    

    //abrimos el servidor
    iniciar_modulo(config_memoria); // Funcion en proceso de creacion!
    //Esta en el utilitis cambiar 

    return 0;
}