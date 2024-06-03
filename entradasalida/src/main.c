#include "io.h"

//t_interfaz *interfaz = NULL;
//t_config_io* config_io = NULL;

void interfaz_conectar(t_interfaz * interfaz) {
    t_paquete *paquete = crear_paquete(CREAR_INTERFAZ);

    agregar_a_paquete(paquete, &interfaz->tipo, sizeof(tipo_interfaz));
    agregar_a_paquete_string(paquete, interfaz->nombre, strlen(interfaz->nombre));
    enviar_paquete(paquete, interfaz->socket_with_kernel);
}

int main(int argc, char *argv[]) {
    t_config_io* config_io = malloc(sizeof(t_config_io)); 
    
    config_io->TIEMPO_UNIDAD_UNIDAD = 0;
    config_io->IP_KERNEL = NULL;
    config_io->PUERTO_KERNEL = NULL;
    config_io->IP_MEMORIA = NULL;
    config_io->PUERTO_MEMORIA = NULL;
    config_io->PATH_BASE_DIALFS = NULL;
    config_io->BLOCK_SIZE = 0;
    config_io->BLOCK_COUNT = 0;
    config_io->RETRASO_COMPACTACION = 0;

    t_interfaz * interfaz;
    char *name_interfaz = argv[0];
    
    char *config_path = "./entradasalida.config";
    //argv[1];
    
    inicializar_interfaz(name_interfaz,config_path,interfaz, config_io);
    interfaz_generar_conexiones_con(interfaz);
    interfaz_conectar(interfaz);
    interfaz_recibir_peticiones(interfaz,config_io);

    return 0;
}

