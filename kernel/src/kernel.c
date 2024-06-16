#include "kernel.h"

t_config_kernel *config_kernel; // La declaros como variable global, y la vez instanciar aca!


int main(int argc, char *argv[]) {
    char *config_path = argv[1];
    
    // Inicializamos logger y cargamos configuracion:
    logger = log_create("kernel.log", "KERNEL", 1, LOG_LEVEL_TRACE);
    config_kernel = cargar_config_kernel(config_path);

    // Generar conexiones con memoria y cpu:
    generar_conexiones_con();

    // Levantamos servidor:
    int socket_servidor = crear_servidor_kernel();

    iniciar_modulo_kernel(socket_servidor);

    return 0;
}