#include "memoria.h"

t_config_memoria* config_memoria;

int main()
{
    //Creando logger
    logger =  log_create("memoria.log","Memoria", 1, LOG_LEVEL_INFO);

    if (logger == NULL)
	{
		perror("No se puedo encontrar el archivo");
		return EXIT_FAILURE;
	}

    config_memoria = inicializar_config_memoria();

    //Inicializamos conexiones
    if (cargar_configuraciones_memoria(config_memoria) != 1)
    { // Generar conexiones, no va a mantener la conexion, sino que va a crear la conexion y la va a cerrar!
        log_error(logger, "Cargar las configuraciones");

        return EXIT_FAILURE;
    }
    
    // Creamos el diccionario de instrucciones por pid 
    lista_instrucciones_porPID = dictionary_create();

    //abrimos el servidor
    iniciar_modulo(config_memoria);
//    cerrar_programa(config_memoria,)

    return 0;
}