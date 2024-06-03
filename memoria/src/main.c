#include "main.h"

void* espacio_usuario;
t_dictionary * diccionario_paginas_porPID;
t_list *tabla_marcos;

    
int main()
{
    t_config_memoria* config_memoria;
    //Creando logger
    logger =  log_create("memoria.log","Memoria", 1, LOG_LEVEL_INFO);

    if (logger == NULL)
	{
		perror("No se puedo encontrar el archivo");
		return EXIT_FAILURE;
	}

    config_memoria = malloc(sizeof(t_config_memoria));
    config_memoria->puerto_escucha = 0;
    config_memoria->tam_memoria = 0;
    config_memoria->tam_pagina = 0;
    config_memoria->path_instrucciones = 0;
    config_memoria->retardo_respuesta = 0;
   
    //Inicializamos conexiones
    if (cargar_configuraciones_memoria(config_memoria) != 1)
    { // Generar conexiones, no va a mantener la conexion, sino que va a crear la conexion y la va a cerrar!
        log_error(logger, "Cargar las configuraciones");

        return EXIT_FAILURE;
    }
    
    //abrimos el servidor
    iniciar_modulo(config_memoria);
    
    //Creamos el diccionario de paginas por PID 
    diccionario_paginas_porPID = dictionary_create();

    //Creamos la tabla de marcos
    int cant_marcos = tam_memoria / tam_pagina;

    bool bitmap[cant_marcos]=NULL

    //tabla_marcos = list_create();


    //Inicializamos los marcos
    inicializar_marcos(cant_marcos);


    return 0;
}