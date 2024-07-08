#include "kernel.h"

t_config_kernel *config_kernel; // La declaros como variable global, y la vez instanciar aca!


int main(int argc, char *argv[]) {
    char *config_path = argv[1];
    
    // Inicializamos logger y cargamos configuracion:
    logger = log_create("kernel.log", "KERNEL", 1, LOG_LEVEL_TRACE);
    logger2 = log_create2("kernel.log", "KERNEL", 1, LOG_LEVEL_MATI);
    config_kernel = cargar_config_kernel(config_path);
    // for(int i = 0; i < string_array_size(config_kernel->RECURSOS); i++) {
    //     log_info(logger, "Recurso: %s", config_kernel->RECURSOS[i]);
    //     log_info(logger, "Cantidad: %i", config_kernel->INST_RECURSOS[i]);
    // }
    // Generar conexiones con memoria y cpu:
    generar_conexiones_con();
    // Levantamos servidor:
    int socket_servidor = crear_servidor_kernel();
    iniciar_modulo_kernel(socket_servidor);
    iniciar_consola();

    prevent_from_memory_leaks();
    log_destroy(logger);

    return 0;
}