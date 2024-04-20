#include "init_cpu.h"

//CARGAMOS LOS DATOS DEL CONFIG
int cargar_configuraciones(t_config_cpu* config_cpu, t_log* logger) {
    t_config* config = config_create("cpu.config");

    if(config_cpu == NULL) {
        log_error(logger, "No se pudo cargar la configuracion del filesystem");

        return -1;
    }

    char* configuraciones[] = {
        "IP_MEMORIA",
        "PUERTO_MEMORIA",
        "PUERTO_ESCUCHA_DISPATCH",
        "PUERTO_ESCUCHA_INTERRUPT",
        "CANTIDAD_ENTRADAS_TLB",
        "ALGORITMO_TLB",
        NULL
    };

    if(!tiene_todas_las_configuraciones(config, configuraciones)) {
        log_error(logger, "No se pudo cargar la configuracion del cpu");

        return -1;
    }
    
    copiar_valor(&config_cpu->ip_memoria, config_get_string_value(config, "IP_MEMORIA"));

    config_cpu->puerto_memoria = config_get_int_value(config, "PUERTO_MEMORIA");
    config_cpu->puerto_escucha_dispatch = config_get_int_value(config, "PUERTO_ESCUCHA_DISPATCH");
    config_cpu->puerto_escucha_interrupt = config_get_int_value(config, "PUERTO_ESCUCHA_INTERRUPT");

    log_info(logger, "Configuraciones cargadas correctamente");
    config_destroy(config);

    return 1;
}

// GENERAMOS CONEXIONES DE CLIENTE A SERVER MEMORIA
int generar_conexiones(t_log* logger, t_config_cpu* config_cpu, int* md_memoria) 
{
    char* puerto_memoria = string_itoa(config_cpu->puerto_memoria);
    char* ip_memoria = config_cpu->ip_memoria;

    *md_memoria = crear_conexion(logger, "MEMORIA", ip_memoria, puerto_memoria); // No harcodearlo! Sino leerlo de kernel.config
    // No existe valores por referencias en C! Primero le sacamos la direccion de memoria a la variable, y despues con *variable asignamos el nuevo valor!

    return (*md_memoria != 0) ? 1 : -1; // Aca pregunto por el nuevo valor!
}

// // CREAMOS SERVIDOR PARA EL CLIENTE KERNEL
 int crear_servidores(t_log* logger, t_config_cpu* config_cpu, int* md_cpu_ds, int* md_cpu_it) 
{
    char* puerto_dispatch = string_itoa(config_cpu->puerto_escucha_dispatch); // Convierte un int a una cadena de char
    char* puerto_interrupt = string_itoa(config_cpu->puerto_escucha_interrupt);
    
    *md_cpu_ds = iniciar_servidor(logger, "DISPATCH", NULL, puerto_dispatch); // Guarda ID del socket
    *md_cpu_it = iniciar_servidor(logger, "INTERRUPT", NULL, puerto_interrupt); 

    return (*md_cpu_ds != 0 && *md_cpu_it != 0) ? 1 : -1;
 }

void iniciar_modulo(t_log* logger_cpu, t_config_cpu* config_cpu) {
    int md_cpu_ds = 0;
    int md_cpu_it = 0;
    
    if(crear_servidores(logger_cpu, config_cpu, &md_cpu_ds, &md_cpu_it) != 1) {
        log_error(logger_cpu, "No se pudo crear los servidores de escucha");

        return;
    }
   
    // pthread_t vector_hilos[2]; -> Sugerecia
    pthread_t hilo_cpu_ds;
    pthread_t hilo_cpu_it;
    t_procesar_conexion* args_ds = crear_procesar_conexion(logger_cpu, "DISPATCH", md_cpu_ds);
    t_procesar_conexion* args_it = crear_procesar_conexion(logger_cpu, "INTERRUPT", md_cpu_it);

    // Notas! -> Seguramente haga un vector de hilos, y que esto vaya en un for, pero por ahora lo dejo asi
    pthread_create(&hilo_cpu_ds, NULL, (void*) server_escuchar_sin_hilos, (void*) args_ds); //Se guarda la info que tenemos antes en el struct
    pthread_create(&hilo_cpu_it, NULL, (void*) server_escuchar_sin_hilos, (void*) args_it);
    
    pthread_join(hilo_cpu_ds, NULL); //Entramos al hilo
    pthread_join(hilo_cpu_it, NULL);
}

void cerrar_programa(t_log* logger, t_config_cpu* config_cpu, int md_memoria) 
{
    log_destroy(logger);
    destruir_configuracion_cpu(config_cpu);
    close(md_memoria);
}

void destruir_configuracion_cpu(t_config_cpu* config_cpu){
    if (config_cpu == NULL) {
        return; // No hay nada que liberar
    }
    free(config_cpu->ip_memoria); // Liberar recursos internos
    free(config_cpu->algoritmo);

    free(config_cpu);   // Liberar la estructura principal
}