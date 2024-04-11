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
    config_kernel->puerto_escucha = NULL;
}

 void* wrapper_crear_servidor(void* args) {
    char* mensaje = (char*) args;
    t_log* logger = log_create("KernelSV.log", "KernelSV", 1, LOG_LEVEL_DEBUG);
    int resultado = crear_servidor(logger, "Kernel", NULL, config_kernel->puerto_escucha); //IP generica
    if (resultado == EXIT_FAILURE) {
        log_error(logger, "Error al crear el servidor");
    }
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
    log_info(logger_kernel,"Hola soy un log");

   inicializar_config(); // Inicializo la variable global config_kernel! -> No se si es la mejor forma de hacerlo!
   

    int md_memoria = 0;
    int md_cpu_dt = 0;
    int md_cpu_it = 0;
    if (!cargar_configuraciones(config_kernel, logger_kernel) || !generar_conexiones(logger_kernel, config_kernel, &md_memoria, &md_cpu_dt, &md_cpu_it))
    { // Generar conexiones, no va a mantener la conexion, sino que va a crear la conexion y la va a cerrar!
        log_error(logger_kernel, "No se pudieron generar las conexiones");

        return 1;
    }

  // iniciar_modulo(); // Funcion en proceso de creacion!

   //cerrar_programa(logger_kernel);
    


    // NEW = crear_cola() // Hay que armar la PCB!
    // READY = crear_cola()
    // EXEC = crear_cola()
    // BLOCK = crear_cola()
    // EXIT = crear_cola()
    
//abrimos el servidor
    pthread_t hilo1,hilo2;
   char* mensaje_1 = "Hilo 1";
   int iret1;
   
    iret1 = pthread_create(&hilo1, NULL, wrapper_crear_servidor, mensaje_1);
    pthread_join(hilo1,NULL);
     
    return 0;
}


    
    



