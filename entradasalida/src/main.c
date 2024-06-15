#include "io.h"

t_interfaz *interfaz = NULL;
t_config_io* config_io = NULL;

void interfaz_conectar() {
    t_paquete *paquete = crear_paquete(CREAR_INTERFAZ);

    agregar_a_paquete(paquete, &interfaz->tipo, sizeof(tipo_interfaz));
    agregar_a_paquete_string(paquete, interfaz->nombre, strlen(interfaz->nombre));
    enviar_paquete(paquete, interfaz->socket_with_kernel);
}

int main(int argc, char *argv[]) {
    char *name_interfaz = argv[0];
    char *config_path = argv[1];

    
    inicializar_interfaz(name_interfaz, config_path);
    interfaz_generar_conexiones_con();
    interfaz_conectar();
    interfaz_recibir_peticiones();

    return 0;
}

