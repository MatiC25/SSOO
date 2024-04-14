#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "init_kernel.h"

t_config_k *config_kernel; // La declaro como variable global, y la vez instanciar aca!

void inicializar_config(void)
{
    config_kernel = malloc(sizeof(t_config_k));
    config_kernel->ip_memoria = NULL;
    config_kernel->ip_cpu = NULL;
    config_kernel->algoritmo_planificacion = NULL;
    config_kernel->inst_recursos = NULL;
    config_kernel->recursos = NULL;
}



int main()
{

    //Creando logger
    t_log* logger_kernel = log_create("kernel.log","Kernel",1, LOG_LEVEL_INFO);
    if ( logger_kernel == NULL)
	{
		perror("No se puedo encontrar el archivo");
		return EXIT_FAILURE;
	}

   inicializar_config(); // Inicializo la variable global config_kernel! -> No se si es la mejor forma de hacerlo!
   
//Inicializamos conexiones

    int md_memoria = 0, md_cpu_dt = 0, md_cpu_it = 0;
    if (cargar_configuraciones(config_kernel, logger_kernel) != 1  || generar_conexiones(logger_kernel, config_kernel, &md_memoria, &md_cpu_dt, &md_cpu_it) != 1 )
    { // Generar conexiones, no va a mantener la conexion, sino que va a crear la conexion y la va a cerrar!
        log_error(logger_kernel, "Cargar las configuraciones");

        return EXIT_FAILURE;
    }


//abrimos el servidor
iniciar_modulo(logger_kernel,config_kernel); // Funcion en proceso de creacion!


cerrar_programa(logger_kernel);
//borrar_conexiones(md_memoria, md_cpu_dt, md_cpu_it)
    
    // NEW = crear_cola() // Hay que armar la PCB!
    // READY = crear_cola()
    // EXEC = crear_cola()
    // BLOCK = crear_cola()
    // EXIT = crear_cola()
    

return 0;
}


    
    



