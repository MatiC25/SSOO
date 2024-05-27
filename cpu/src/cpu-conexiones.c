#include "cpu-conexiones.h"

// GENERAMOS CONEXIONES DE CLIENTE A SERVER MEMORIA
void* generar_conexion_a_memoria(void* arg) {
    int md_memoria = 0;
    char* puerto_memoria = string_itoa(config_cpu->PUERTO_MEMORIA);
    char* ip_memoria = config_cpu->IP_MEMORIA;

    if (ip_memoria == NULL){
        log_error(logger, "IP_MEMORIA es NULL");
        free(puerto_memoria); // Liberar memoria asignada por string_itoa
        return NULL;
    }
    

    md_memoria = crear_conexion("MEMORIA", ip_memoria, puerto_memoria); // No harcodearlo! Sino leerlo de kernel.config
    free(puerto_memoria);

    if(md_memoria == -1) {
        log_error(logger, "No se pudo conectar a la memoria");
        return NULL;
    }
    
    config_cpu->SOCKET_MEMORIA = md_memoria;
    return NULL;
}

int generar_servidor_cpu_dispatch() {
    char* puerto_dispatch = string_itoa(config_cpu->PUERTO_ESCUCHA_DISPATCH); // Convierte un int a una cadena de char
    int md_cpu_ds = iniciar_servidor("DISPATCH", NULL, puerto_dispatch); // Guarda ID del socket
    free(puerto_dispatch);
    iniciar_ciclo_de_ejecucion(md_cpu_ds);

    return md_cpu_ds;
}

int generar_servidor_cpu_interrupt() {
    pthread_t hilo_interrupt;
    char* puerto_interrupt = string_itoa(config_cpu->PUERTO_ESCUCHA_INTERRUPT);
    int md_cpu_it = iniciar_servidor("INTERRUPT", NULL, puerto_interrupt); 
    free(puerto_interrupt);

    t_procesar_server* args = malloc(sizeof(t_procesar_server));
    args->server_name = "INTERRUPT";
    args->socket_servidor = md_cpu_it;

    pthread_create(&hilo_interrupt, NULL, server_escuchar_sin_hilos, (void*)args);
    pthread_detach(hilo_interrupt); // Usar pthread_detach para no esperar al hilo

    return md_cpu_it;
}

// // CREAMOS SERVIDOR PARA EL CLIENTE KERNEL
void crear_servidores_cpu(int *md_cpu_ds,int *md_cpu_it) {
    *md_cpu_it = generar_servidor_cpu_interrupt();
    *md_cpu_ds = generar_servidor_cpu_dispatch();
    

    if(*md_cpu_ds == -1 || *md_cpu_it == -1) {
        log_error(logger, "No se pudo crear los servidores de escucha");
    }
     
}