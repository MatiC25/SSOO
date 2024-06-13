#include "memoria.h"

// variables globales (o externas)
t_dictionary * diccionario_paginas_porPID;
t_dictionary* lista_instrucciones_porPID;
t_config_memoria* config_memoria;
t_bitarray* bitmap;
t_list *tabla_marcos;
char *path_proceso;
void* espacio_de_usuario;

    
int main()
{
    
    // Creando logger
    logger =  log_create("memoria.log","Memoria", 1, LOG_LEVEL_INFO);

    if (logger == NULL)
	{
		perror("No se puedo encontrar el archivo");
		return EXIT_FAILURE;
	}

    config_memoria = inicializar_config_memoria();
    
    // Inicializamos conexiones
    if (cargar_configuraciones_memoria(config_memoria) != 1)
    { // Generar conexiones, no va a mantener la conexion, sino que va a crear la conexion y la va a cerrar!
        log_error(logger, "Cargar las configuraciones");

        return EXIT_FAILURE;
    }
    
    // abrimos el servidor
    iniciar_modulo(config_memoria);
    
    //Creamos el diccionario de paginas por PID 
    diccionario_paginas_porPID = dictionary_create();

    // memoria de usuario y bitmap
    int cantidad_marcos = config_memoria->tam_memoria / config_memoria->tam_memoria; //calculamos la cantidad de marcos
    espacio_de_usuario = malloc (config_memoria->tam_memoria); // reservamos memoria para el espacio de usuarip
    void* memoria_usuario_bitmap = malloc (cantidad_marcos/8);// reservamos memoria para el bitmap
    bitmap = bitarray_create_with_mode (memoria_usuario_bitmap , cantidad_marcos/8, LSB_FIRST);

    return 0;
}