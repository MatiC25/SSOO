#include "io.h"

t_interfaz *interfaz;

int main(int argc, char *argv[]) {
    char *name_interfaz = argv[1];
    char *config_path = argv[2];

    interfaz = inicializar_interfaz(name_interfaz, config_path);
    interfaz_generar_conexiones(interfaz);
    interfaz_conectar(interfaz);
    interfaz_recibir_peticiones(interfaz);

    return 0;
}