#include <stdlib.h>
#include <stdio.h>
#include "init_kernel.h"

t_config_k *config_kernel; // La declaro como variable global, y la vez instanciar aca!

void inicializar_config(void)
{
    config_kernel = malloc(sizeof(t_config_k));
    config_kernel->ip_memoria = NULL;
    config_kernel->ip_cpu = NULL;
    config_kernel->ip_fs = NULL;
    config_kernel->algoritmo_planificacion = NULL;
    config_kernel->inst_recursos = NULL;
    config_kernel->recursos = NULL;
}

int main()
{
    t_log *logger_kernel = log_create("kernel.log", "KERNEL", true, LOG_LEVEL_INFO);
    inicializar_config(); // Inicializo la variable global config_kernel! -> No se si es la mejor forma de hacerlo!

    // NEW = crear_cola() // Hay que armar la PCB!
    // READY = crear_cola()
    // EXEC = crear_cola()
    // BLOCK = crear_cola()
    // EXIT = crear_cola()

    int md_memoria = 0, md_cpu_dt = 0, md_cpu_it = 0, md_fs = 0;

    if (!cargar_configuraciones(config_kernel, logger_kernel) || !generar_conexiones(logger_kernel, config_kernel, &md_memoria, &md_cpu_dt, &md_cpu_it, &md_fs))
    { // Generar conexiones, no va a mantener la conexion, sino que va a crear la conexion y la va a cerrar!
        log_error(logger_kernel, "No se pudieron generar las conexiones");

        return 1;
    }

    // Fede en momento de insipiracion!
    // iniciar_modulo(); // Funcion en proceso de creacion!

    cerrar_programa(logger_kernel);

    return 0;
}
