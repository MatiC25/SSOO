#include "init_memoria.h"

t_config_memoria* inicializar_config_memoria(void){

    t_config_memoria* config_memoria = malloc(sizeof(t_config_memoria));
    config_memoria->puerto_escucha = 0;
    config_memoria->tam_memoria = 0;
    config_memoria->tam_pagina = 0;
    config_memoria->path_instrucciones = NULL;
    config_memoria->retardo_respuesta = 0;
    return config_memoria;
}

int cargar_configuraciones_memoria(t_config_memoria* config_memoria) {
    t_config* config = config_create("memoria.config");
    if (config == NULL) {
        log_error(logger, "No se pudo cargar la configuración de memoria");
        return -1;
    }

    char* configuraciones[] = {
        "PUERTO_ESCUCHA",
        "TAM_MEMORIA",
        "TAM_PAGINA",
        "PATH_INSTRUCCIONES",
        "RETARDO_RESPUESTA",
        NULL
    };

    if (!tiene_todas_las_configuraciones(config, configuraciones)) {
        log_error(logger, "Faltan configuraciones en memoria.config");
        config_destroy(config);
        return -1;
    }
    config_memoria->puerto_escucha = config_get_int_value(config, "PUERTO_ESCUCHA");
    config_memoria->tam_memoria = config_get_int_value(config, "TAM_MEMORIA");
    config_memoria->tam_pagina = config_get_int_value(config, "TAM_PAGINA");
    copiar_valor(&config_memoria->path_instrucciones, config_get_string_value(config, "PATH_INSTRUCCIONES"));
    config_memoria->retardo_respuesta = config_get_int_value(config, "RETARDO_RESPUESTA");

    log_info(logger, "Configuraciones cargadas correctamente");
    config_destroy(config);
    return 1;
}

void config_destroy_version_memoria(t_config_memoria* config_memoria) {
    if (config_memoria->path_instrucciones != NULL) {
        free(config_memoria->path_instrucciones);
    }
    free(config_memoria);
}

int crear_servidores(t_config_memoria* config_memoria, int *md_generico) {
    char* puerto_memoria = string_itoa(config_memoria->puerto_escucha);
    *md_generico = iniciar_servidor("MEMORIA", NULL, puerto_memoria);
    free(puerto_memoria); // Liberamos la memoria utilizada por string_itoa
    return (*md_generico != 0) ? 1 : -1;
}

int iniciar_modulo(t_config_memoria* config_memoria) {
    int md_generico = 0;
    if (crear_servidores(config_memoria, &md_generico) != 1) {
        log_error(logger, "No se pudo crear los servidores de escucha");
        return -1;
    }

    while (1) {
        int socket_cliente = esperar_cliente("MEMORIA", md_generico);
        log_warning(logger,"socket_cliente %i", socket_cliente);
        if (socket_cliente != -1) {
            while(1) {
                op_code cod_op = recibir_operacion(socket_cliente);
                switch(cod_op) {
                case HANDSHAKE:
                    recibir_handshake(socket_cliente);
                    break;
                case MENSAJE:
                    recibir_mensaje(socket_cliente);
                    log_warning(logger,"enviado mensaje");
                    enviar_mensaje("MEMORIA -> CPU", socket_cliente);
                    break;
                    case HANDSHAKE_PAGINA:
                    recibir_handshake(socket_cliente);
                    handshake_desde_memoria(socket_cliente);
                    break;
                case -1:
                    log_info(logger, "Se desconectó el cliente");
                    close(socket_cliente);
                    return;
                default:
                    log_error(logger, "Operación desconocida");
                }
            }
        }
    }

    return md_generico;
}

void cerrar_programa(t_config_memoria* config_memoria, int socket_server) {
    config_destroy_version_memoria(config_memoria);
    close(socket_server);
}


// void* escuchar_peticiones(void* args) {
//     int socket_cliente = *(int*) args; // Convertimos los argumentos de nuevo a int
//     free(args);
//     while (1) {
//         int cod_op = recibir_operacion(socket_cliente);
//         switch (cod_op) {
//         case MENSAJE:
//         recibir_mensaje(socket_cliente);
//         //enviar_mensaje("sdkfhasz", socket_cliente);
//             break;
//             case HANDSHAKE:
//                 recibir_handshake(socket_cliente);
//                 //enviar_mensaje("sdkfhasz", socket_cliente);
//                 break;
//             case HANDSHAKE_PAGINA:
//                 recibir_handshake(socket_cliente);
//                 handshake_desde_memoria(socket_cliente);
//                 break;
//             case INICIAR_PROCESO:
//                 //retardo_pedido(config_memoria->retardo_respuesta);
//                 //crear_proceso(socket_cliente);
//                 //leer_archivoPseudo(socket_cliente);
//                 break;
//             case FINALIZAR_PROCESO:
//                 //retardo_pedido(config_memoria->retardo_respuesta);
//                 //terminar_proceso(socket_cliente);
//                 break;
//             case INSTRUCCION: 
//                 // El retardo ya está incluido en la función
//                 // enviar_instruccion_a_cpu(socket_cliente);
//                 break;
//             case ACCEDER_TABLA_PAGINAS: 
//                 //retardo_pedido(config_memoria->retardo_respuesta);
//                 //obtener_marco(socket_cliente);
//                 break;
//             case MODIFICAR_TAMAÑO_MEMORIA:
//                 //retardo_pedido(config_memoria->retardo_respuesta);
//                 //resize_proceso(socket_cliente);
//                 break;
//             case ACCESO_A_LECTURA:
//                 //retardo_pedido(config_memoria->retardo_respuesta);
//                 //acceso_lectura(socket_cliente);
//                 break;
//             case ACCESO_A_ESCRITURA:
//                 //retardo_pedido(config_memoria->retardo_respuesta);
//                 //acceso_escritura(socket_cliente);
//                 break;
//             case -1:
//                 log_info(logger, "Se desconectó el cliente");
//                 free(args); // Liberamos la memoria asignada a los argumentos
//                 close(socket_cliente);
//                 return NULL;
//             default:
//                 log_error(logger, "Operación desconocida");
//         }
//     }

//     // Cerramos el socket del cliente y liberamos la memoria asignada a los argumentos
//     close(socket_cliente);
//     return NULL;
// }           

void handshake_desde_memoria(int socket_cliente) {
    t_paquete* paquete = crear_paquete(HANDSHAKE_PAGINA);
    int tam_pagina = config_memoria ->tam_pagina;

    // send(socket_cliente, &tam_pagina, sizeof(int), 0);
    
    agregar_a_paquete(paquete, &tam_pagina, sizeof(int));
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}