#include "io.h"

int main(int argc, char *argv[]) {
    name_interfaz = "GENERICA";
    config_path = "./entradasalida.config";

    logger = log_create("io.log", "IO", 1, LOG_LEVEL_TRACE);
    interfaz = inicializar_interfaz(name_interfaz, config_path);

    configurar_senial_cierre();
    // interfaz_generar_conexiones_con(interfaz);
    // send_interfaz_a_kernel(interfaz);
    interfaz_recibir_peticiones(interfaz);

    return 0;
}