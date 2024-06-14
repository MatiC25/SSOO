#include "inicializar_memoria.h"

int crear_servidores(t_config_memoria* config_memoria, int *md_generico) {
    char* puerto_memoria = string_itoa(config_memoria->puerto_escucha); // Convierte un int a una cadena de char
    //Linux tarda
    *md_generico = iniciar_servidor("MEMORIA", "127.0.0.1", puerto_memoria);
    free(puerto_memoria);
    return (*md_generico != 0) ? 1 : -1;
}

int iniciar_modulo(t_config_memoria* config_memoria) {
    int md_generico = 0;

    if(crear_servidores(config_memoria, &md_generico) != 1) {
        log_error(logger, "No se pudo crear los servidores de escucha");
        return -1;
    }

    while (1)
    {
        int socket_cliente = esperar_cliente("MEMORIA", md_generico);

        if(socket_cliente != -1) 
        {
          
			pthread_t hilo_memora;
			t_procesar_conexion *args_hilo = crear_procesar_conexion("MEMORIA", socket_cliente);
            

            pthread_create(&hilo_memora, NULL, escuchar_peticiones, (void *) args_hilo);
            pthread_detach(hilo_memora);
        }
    
    }
    return md_generico;

    cerrar_programa(config_memoria, md_generico);
}

void cerrar_programa(t_config_memoria* config_memoria, int socket_server){
    config_destroy_version_memoria(config_memoria);
    close(socket_server);
}