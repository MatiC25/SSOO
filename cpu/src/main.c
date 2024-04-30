#include <stdlib.h>
#include <stdio.h>
#include "init_cpu.h"

t_config_cpu *config_cpu; // La declaro como variable global, y la vez instanciar aca!

void inicializar_config(void)
{
    config_cpu = malloc(sizeof(t_config_cpu));
    config_cpu -> ip_memoria = NULL;
    config_cpu -> algoritmo = NULL;
}


int main(void)
{

    //Creando logger
    logger = log_create("CPUlog.log","CPU",1, LOG_LEVEL_INFO);

    if ( logger == NULL)
	{
		perror("No se puedo encontrar el archivo");
		return EXIT_FAILURE;
	}

    inicializar_config(); // Inicializo la variable global config_kernel! -> No se si es la mejor forma de hacerlo!

    int md_memoria = 0;

    if(cargar_configuraciones(config_cpu) != 1 || generar_conexiones(config_cpu, &md_memoria) != 1)
    {
        log_error(logger, "Error al cargar el .config");
        return EXIT_FAILURE;
    }

    //Inicio de server 
    iniciar_modulo(config_cpu);

    char *valor = "hola";

    //Envio de primer mensaje
    enviar_mensaje(valor, md_memoria);

    //Ecvios de paquetes
    paquete(md_memoria);
    
    cerrar_programa(config_cpu, md_memoria);

    return EXIT_SUCCESS;
}


