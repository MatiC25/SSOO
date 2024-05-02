#include "cpu-conexiones.h"

// GENERAMOS CONEXIONES DE CLIENTE A SERVER MEMORIA
void generar_conexion_a_memoria() {
    int md_memoria = 0;
    char* puerto_memoria = string_itoa(config_cpu->puerto_memoria);
    char* ip_memoria = config_cpu->ip_memoria;

    md_memoria = crear_conexion("MEMORIA", ip_memoria, puerto_memoria); // No harcodearlo! Sino leerlo de kernel.config

    if(md_memoria == -1) {
        log_error(logger, "No se pudo conectar a la memoria");

        exit(-1);
    }

    config_cpu -> SOCKET_MEMORIA = md_memoria;
}

int generar_servidor_cpu_dispatch() {
    char* puerto_dispatch = string_itoa(config_cpu->puerto_escucha_dispatch); // Convierte un int a una cadena de char
    int md_cpu_ds = iniciar_servidor("DISPATCH", NULL, puerto_dispatch); // Guarda ID del socket

    return md_cpu_ds;
}

int generar_servidor_cpu_interrupt() {
    pthread_t hilo_interrupt;
    char* puerto_interrupt = string_itoa(config_cpu->puerto_escucha_interrupt);
    int md_cpu_it = iniciar_servidor("INTERRUPT", NULL, puerto_interrupt); 

    pthread_create(&hilo_interrupt, NULL, (void*)escuchar_conexiones, (void*)md_cpu_it);

    return md_cpu_it;
}

// // CREAMOS SERVIDOR PARA EL CLIENTE KERNEL
int crear_servidores(int* md_cpu_ds, int* md_cpu_it) {
    int md_cpu_ds = generar_servidor_cpu_dispatch();
    int md_cpu_it = generar_servidor_cpu_interrupt();

    if(md_cpu_ds == -1 || md_cpu_it == -1) {
        log_error(logger, "No se pudo crear los servidores de escucha");

        return -1;
    }
    
    return 1;
}