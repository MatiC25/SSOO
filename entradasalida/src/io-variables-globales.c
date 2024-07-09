#include "io-variables-globales.c"

// Definimos las variables globales:
t_list *archivos_ya_abiertos = NULL;
t_bitarray *bitmap = NULL;
FILE *archivo_bitmap = NULL;
FILE *archivo_bloque = NULL;
t_interfaz *interfaz = NULL;

// Funcion para cerrar programa:
void cerrar_programa(int signal) {
    if(signal == SIGINT) {
        log_info(logger, "Cerrando programa...");
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
