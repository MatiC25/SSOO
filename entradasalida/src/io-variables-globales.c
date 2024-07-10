#include "io-variables-globales.h"

// Definimos las variables globales:
t_list *archivos_ya_abiertos = NULL;
t_bitarray *bitmap = NULL;
FILE *archivo_bitmap = NULL;
FILE *archivo_bloque = NULL;
t_interfaz *interfaz = NULL;
char *name_interfaz = NULL;
char *config_path = NULL;

// Funcion para cerrar programa:
void cerrar_programa(int signal) {
    if(signal == SIGINT) {
        log_info(logger, "Cerrando programa...");
        
        // Finalizamos el programa:
        if(interfaz->tipo == DIALFS) {
            cerrar_todos_los_archivos_abiertos(archivos_ya_abiertos);
            cerrar_bitmap(bitmap);
            fclose(archivo_bitmap);
            fclose(archivo_bloque);
        }

        cerrar_sockets(interfaz);
        liberar_interfaz(interfaz);
        
        exit(0);
    }
}

// Funcion para configurar la senial de cierre:
void configurar_senial_cierre() {
    struct sigaction sa;
    sa.sa_handler = cerrar_programa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error al configurar la senial de cierre");
        exit(1);
    }
}