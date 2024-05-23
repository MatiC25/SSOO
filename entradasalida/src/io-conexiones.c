#include "io-conexiones.h"

void interfaz_generar_conexiones_con(t_interfaz *interfaz) {
    generar_conexiones_con_kernel(interfaz);

    if(interfaz->tipo != GENERICA)
        generar_conexiones_con_memoria(interfaz);   
}

void generar_conexiones_con_kernel(t_interfaz *interfaz) {
    int socket_kernel;

    char *nombre_interfaz = interfaz->nombre;
    char *ip_kernel = interfaz->config->ip_kernel;
    char *puerto_kernel = interfaz->config->puerto_kernel;

    generar_conexion(socket_kernel, nombre_interfaz, ip_kernel, puerto_kernel);

    interfaz->socket_kernel = socket_kernel;
}

void generar_conexiones_con_memoria(t_interfaz *interfaz) {
    int socket_memoria;

    char *nombre_interfaz = interfaz->nombre;
    char *ip_memoria = interfaz->config->ip_memoria;
    char *puerto_memoria = interfaz->config->puerto_memoria;

    generar_conexion(socket_memoria, nombre_interfaz, ip_memoria, puerto_memoria);

    interfaz->socket_memoria = socket_memoria;
}

void generar_conexion(int *socket, char *nombre_interfaz, char *ip, char *puerto) {
    socket = crear_conexion(nombre_interfaz, ip, puerto);

    if(socket == 0 || socket == -1)
        exit(EXIT_FAILURE);

    generar_handshake(socket, nombre_interfaz, ip, puerto);
}

