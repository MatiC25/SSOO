#include "kernel-estructuras.h"

t_config_kernel *config_kernel; // La declaros como variable global, y la vez instanciar aca!

int main(int argc, char *argv[]) {
    char *mensaje;
    int tam;

    char *config_path = argv[1];
    
    // Inicializamos logger y cargamos configuracion:
    logger = log_create("kernel.log", "KERNEL", 1, LOG_LEVEL_TRACE);
    config_kernel = cargar_config_kernel(config_path);

    // Generar conexiones con memoria y cpu:
    generar_conexiones_con();

    // Levantamos servidor:
    // int socket_servidor = crear_servidor_kernel();
    //recv(config_kernel->SOCKET_DISPATCH, &resultado, sizeof(int), MSG_WAITALL);
    //log_info(logger, "Recibido: %d", resultado);
    //mensaje = malloc(resultado);
    //recv(config_kernel->SOCKET_DISPATCH, mensaje, resultado, MSG_WAITALL);
    log_info(logger, "Recibido: %s", mensaje);

    return 0;
}