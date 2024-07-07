#include "io.h"

t_interfaz *interfaz = NULL;

int main(int argc, char *argv[]) {
    char *name_interfaz = "TECLADO";
    char *config_path = "./entradasalida.config";

    // Inicializamos logger y interfaz:
    logger = log_create("io.log", "IO", 1, LOG_LEVEL_TRACE);
    interfaz = inicializar_interfaz(name_interfaz, config_path);

    interfaz_generar_conexiones_con(interfaz);
    send_interfaz_a_kernel(interfaz);
    interfaz_recibir_peticiones(interfaz);

    return 0;
}