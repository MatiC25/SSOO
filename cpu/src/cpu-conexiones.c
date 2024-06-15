#include "cpu-conexiones.h"

atomic_int interrupt_flag = ATOMIC_VAR_INIT(0);

// GENERAMOS CONEXIONES DE CLIENTE A SERVER MEMORIA
void* generar_conexion_a_memoria(void* arg) {
    char* puerto_memoria = config_cpu->PUERTO_MEMORIA;
    char* ip_memoria = config_cpu->IP_MEMORIA;

    log_info(logger, "%s", ip_memoria);
    log_info(logger, "%s", puerto_memoria);
    
    if (!ip_memoria){
        log_error(logger, "IP_MEMORIA es NULL");
        return NULL;
    }

    int md_memoria = crear_conexion("MEMORIA", ip_memoria, puerto_memoria);
    log_warning(logger,"md_memoria: %i",md_memoria);
    if (md_memoria == -1) {
        log_error(logger, "Error al conectar con MEMORIA");
        return NULL;
    }

    config_cpu->SOCKET_MEMORIA = md_memoria;
    log_warning(logger,"enviado mensaje");
    enviar_mensaje("Hola, soy el CPU", md_memoria);
    
    log_warning(logger,"reciviendo mensaje");
    // Recibir mensaje de la memoria:    
    op_code cod_op = recibir_operacion(md_memoria);
    log_info(logger, "Código de operación recibido: %i", cod_op);
    recibir_mensaje(md_memoria);
    

    generar_handshake(md_memoria, "MEMORIA", ip_memoria, puerto_memoria);
    generar_handshake_para_pagina(md_memoria, "MEMORIA", ip_memoria, puerto_memoria);

    return NULL;
}

void generar_handshake_para_pagina(int socket, char *server_name, char *ip, char *puerto) {
    int32_t handshake = 1;
    int32_t result;
    op_code cod_op = HANDSHAKE_PAGINA;

    // Enviar código de operación HANDSHAKE_PAGINA
    send(socket, &cod_op, sizeof(op_code), 0);

    // Enviar handshake
    send(socket, &handshake, sizeof(int32_t), 0);

    // Recibir respuesta del servidor
    recv(socket, &result, sizeof(int32_t), MSG_WAITALL);

    if (!result) { 
        // Handshake exitoso
        log_info(logger, "Handshake exitoso con página %s", server_name);
        
        // Recibir tamaño de página desde el servidor
        int tam_pagina = recv_pagina(socket);
        // config_cpu->TAMANIO_PAGINA = recv_pagina(socket);
        log_info(logger, "Tamaño de página recibido: %i", tam_pagina);
    } else {
        // Error en el handshake
        log_error(logger, "Error en el handshake con %s", server_name);
        exit(EXIT_FAILURE); // Terminar el programa en caso de error crítico
    }
}

int generar_servidor_cpu_dispatch() {
    char* puerto_dispatch = config_cpu->PUERTO_ESCUCHA_DISPATCH;
    if (!puerto_dispatch) {
        log_error(logger, "El puerto de escucha DISPATCH no está configurado");
        return -1;
    }

    int md_cpu_ds = iniciar_servidor("DISPATCH", NULL, puerto_dispatch);
    free(puerto_dispatch);

    if (md_cpu_ds == -1) {
        log_error(logger, "Error al iniciar el servidor DISPATCH");
        return -1;
    }

    while (1) {
        int socket_cliente = esperar_cliente("DISPATCH", md_cpu_ds);
        if (socket_cliente != -1) {
            config_cpu->SOCKET_KERNEL = socket_cliente;
            log_info(logger, "Cliente conectado al servidor DISPATCH: %i", config_cpu->SOCKET_KERNEL);
            iniciar_ciclo_de_ejecucion(md_cpu_ds, socket_cliente);
        }
    }

    return md_cpu_ds;
}

int generar_servidor_cpu_interrupt() {
    char* puerto_interrupt = config_cpu->PUERTO_ESCUCHA_INTERRUPT;
    if (!puerto_interrupt) {
        log_error(logger, "El puerto de escucha INTERRUPT no está configurado");
        return -1;
    }

    int md_cpu_it = iniciar_servidor("INTERRUPT", NULL, puerto_interrupt);
    free(puerto_interrupt);

    if (md_cpu_it == -1) {
        log_error(logger, "Error al iniciar el servidor INTERRUPT");
        return -1;
    }

    pthread_t hilo_interrupt;
    t_procesar_server* args = malloc(sizeof(t_procesar_server));
    if (!args) {
        log_error(logger, "Error al asignar memoria para los argumentos del hilo INTERRUPT");
        close(md_cpu_it);
        return -1;
    }

    args->server_name = "INTERRUPT";
    args->socket_servidor = md_cpu_it;

    pthread_create(&hilo_interrupt, NULL, server_interrupt, (void*)args);
    pthread_detach(hilo_interrupt);

    return md_cpu_it;
}

void crear_servidores_cpu(int *md_cpu_ds, int *md_cpu_it) {
    *md_cpu_it = generar_servidor_cpu_interrupt();
    *md_cpu_ds = generar_servidor_cpu_dispatch();

    if (*md_cpu_ds == -1 || *md_cpu_it == -1) {
        log_error(logger, "No se pudo crear los servidores de escucha");
    }
}

void* server_interrupt(void* args) {
    t_procesar_server* arg = (t_procesar_server*) args;
    char* server_name = arg->server_name;
    int socket_server = arg->socket_servidor;

    while (1) {
        int socket_cliente = esperar_cliente(server_name, socket_server);
        if (socket_cliente == -1) {
            log_error(logger, "Error al esperar cliente en %s", server_name);
            continue; // O salir del bucle dependiendo de la lógica deseada
        }

        while (1) {
            int cod_op = recibir_operacion(socket_cliente);

            switch (cod_op) {
                case MENSAJE:
                    recibir_mensaje(socket_cliente);
                    enviar_mensaje("Hola, soy el CPU", socket_cliente);
                    break;
                case HANDSHAKE:
                    recibir_handshake(socket_cliente);
                    break;
                case FINQUANTUM:
                    atomic_store(&interrupt_flag, 1);
                    break;
                case -1:
                    log_info(logger, "Se desconectó el cliente Kernel (IT)");
                    close(socket_cliente);
                    break;
                default:
                    log_error(logger, "Operación desconocida");
                    break;
            }
        }
    }
}