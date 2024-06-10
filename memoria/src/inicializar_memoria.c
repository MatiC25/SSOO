#include "inicializar_memoria.h"


t_config_memoria* config_memoria; // Variable global
//SEPARAR EN 2 
//un archivo config y otro conexiones 

int cargar_configuraciones_memoria(t_config_memoria* config_memoria) { //(char *path_config_memoria
    t_config* config = config_create("memoria.config");

    if(!config) {
        log_error(logger, "No se pudo cargar la configuracion de memoria");

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
    
    if(!tiene_todas_las_configuraciones(config, configuraciones)) {
        log_error(logger, "No se pudo cargar la configuracion de la memoria");

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

int crear_servidores(t_config_memoria* config_memoria, int *md_generico) {
    char* puerto_memoria = string_itoa(config_memoria->puerto_escucha); // Convierte un int a una cadena de char
    //Linux tarda
    *md_generico = iniciar_servidor("MEMORIA", "127.0.0.1", puerto_memoria);
    
    free(puerto_memoria);
    return (*md_generico != 0) ? 1 : -1;
}

void iniciar_modulo(t_config_memoria* config_memoria) {
    int md_generico = 0;
    char* server_name;

    if(crear_servidores(config_memoria, &md_generico) != 1) {
        log_error(logger, "No se pudo crear los servidores de escucha");
        return;
    }
    
    while (1)
    {
        
        int socket_cliente = esperar_cliente(server_name, md_generico);
        if(socket_cliente != -1) 
        {
          
			pthread_t hilo_memora;
			t_procesar_conexion *args_hilo = crear_procesar_conexion(server_name, socket_cliente);
            

            pthread_create(&hilo_memora, NULL, escuchar_peticiones, (void *) args_hilo);
            pthread_detach(hilo_memora);
        }
        
    }
    return md_generico;

    cerrar_programa(config_memoria, md_generico);
}

void cerrar_programa(t_config_memoria *config_memoria, int socket_server){
    config_destroy(config_memoria);
    close(socket_server);
}

void* escuchar_peticiones(void* args) {
    t_procesar_server* args_hilo = (t_procesar_server*) args;
    char* server_name = args_hilo->server_name;
    int socket_cliente = args_hilo->socket_servidor;


    while (1)
    {
        op_code cod_op = recibir_operacion(socket_cliente);
        
        switch (cod_op)
        {
        case HANDSHAKE:
            //generar_handshake(socket_cliente);
            break;
        //case HANDSHAKE_PAGINA:
            //handshake_desde_memoria(socket_cliente);
          //  break;
        case INICIAR_PROCESO: 
            //retardo_pedido(config_memoria -> retardo_de_respuesta);
            //crear_proceso(socket_cliente);
            //leer_archivoPseudo(socket_cliente);
            break;
        case FINALIZAR_PROCESO:
            //sretardo_pedido(config_memoria -> retardo_de_respuesta);
            //terminar_proceso(socket_cliente);
            break;
        case INSTRUCCION: 
            log_info(logger,"%i", socket_cliente);
            enviar_instruccion_a_cpu(socket_cliente,10);
            //El retardo ya esta oincluido en la funcion
            //enviar_instruccion_a_cpu(socket_cliente, config_memoria -> retardo_de_respuesta);
            break;
        case ACCEDER_TABLA_PAGINAS: 
            //retardo_pedido(config_memoria -> retardo_de_respuesta);
            //obtener_marco(socket_cliente);
            break;
        case MODIFICAR_TAMAÑO_MEMORIA:
            //retardo_pedido(config_memoria -> retardo_de_respuesta);
            //resize_proceso(socket_cliente);
            break;
        case ACCESO_A_LECTURA:
            //retardo_pedido(config_memoria -> retardo_de_respuesta);
            //acceso_lectura(socket_cliente);
            break;
        case ACCESO_A_ESCRITURA:
            //retardo_pedido(config_memoria -> retardo_de_respuesta);
            //(socket_cliente);
            break;
        default:
            //log_error(logger, "Operacion desconocida");
            break;
        }
    }
}