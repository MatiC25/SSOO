#include "instrucciones.h"

void send_instruccion(int socket_cliente, char *instruccion, t_list *parametros) {
    t_tipo_instruccion tipo_instruccion = obtener_tipo_instruccion(instruccion);
    t_paquete *paquete = crear_paquete(tipo_instruccion);

    agregar_a_paquete_lista_string(paquete, parametros);
    eliminar_paquete(paquete);
}


t_instruccion* recv_instruccion(int socket_cliente){
    op_code operacion = recibir_operacion(socket_cliente);
    t_list *parametros = list_create();

    int size;
    int desplazamiento = 0;
    
    void* buffer = recibir_buffer(&size, socket_cliente);
    
    if(buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
        free(buffer);
    }
    
    int tamanio;
    char *parametro;

   while (desplazamiento < size) {
        int tamanio;
        // Copiar el tamaño del siguiente parámetro desde el buffer
        memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);

        // Asignar memoria para el parámetro, incluyendo el carácter nulo
        char* parametro = malloc(tamanio + 1);
        if (parametro == NULL) {
            perror("Error al asignar memoria");
            exit(EXIT_FAILURE);
        }

        // Copiar el parámetro desde el buffer
        memcpy(parametro, buffer + desplazamiento, tamanio);
        desplazamiento += tamanio + 1;

        // Asegurarse de que la cadena esté terminada en nulo
        parametro[tamanio] = '\0';

        // Loguear el parámetro y su tamaño
        //log_info(logger, "Parametro: %s", parametro);
        //log_info(logger, "Tamanio: %i", tamanio);

        // Agregar el parámetro a la lista
        list_add(parametros, parametro);
    }

    free(buffer);

    t_instruccion *instruccion = crear_instrucciones(parametros);
    return instruccion;
}

t_instruccion *crear_instrucciones(t_list *parametros) {
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));

    for(int i = 0; i < list_size(parametros); i++) {
        char *parametro = list_get(parametros, i);

        if(i == 0) {
            instruccion->opcode = parametro;
        } else if(i == 1) {
            instruccion->parametro1 = parametro;
        } else if(i == 2) {
            instruccion->parametro2 = parametro;
        } else if(i == 3) {
            instruccion->parametro3 = parametro;
        } else if(i == 4) {
            instruccion->parametro4 = parametro;
        } else if (i == 5){
            instruccion->parametro5 = parametro;
        }
    }

    return instruccion;
}

void solicitar_instruccion(int socket_server, int PID, int program_counter) {
    t_paquete *paquete = crear_paquete(INSTRUCCION);
    agregar_a_paquete(paquete, &PID, sizeof(int));
    agregar_a_paquete(paquete, &program_counter, sizeof(int));
    
    enviar_paquete(paquete, socket_server);
    eliminar_paquete(paquete);
}

t_tipo_instruccion obtener_tipo_instruccion(char* instruccion) {
     // Usa esa funcion es la comparacion de strings
    //string_equals_ignore_case

    if(strcmp(instruccion, "SET") == 0) {
        return SET;
    } else if(strcmp(instruccion, "MOVE_IN") == 0) {
        return MOVE_IN;
    } else if(strcmp(instruccion, "MOV_OUT") == 0) {
        return MOV_OUT;
    } else if(strcmp(instruccion, "SUM") == 0) {
        return SUM;
    } else if(strcmp(instruccion, "SUB") == 0) {
        return SUB;
    } else if(strcmp(instruccion, "JNZ") == 0) {
        return JNZ;
    } else if(strcmp(instruccion, "RESIZE") == 0) {
        return RESIZE;
    }else if(strcmp(instruccion, "COPY_STRING") == 0) {
        return COPY_STRING;
    }else if(strcmp(instruccion, "IO_GEN_SLEEP") == 0) {
        return IO_GEN_SLEEP;
    }else if(strcmp(instruccion, "IO_STDIN_READ") == 0) {
        return IO_STDIN_READ;
    }else if(strcmp(instruccion, "IO_STDOUT_WRITE") == 0) {
        return IO_STDOUT_WRITE;
    }else if(strcmp(instruccion, "IO_FS_CREATE") == 0) {
        return IO_FS_CREATE;
    }else if(strcmp(instruccion, "IO_FS_DELETE") == 0) {
        return IO_FS_DELETE;
    }else if(strcmp(instruccion, "IO_FS_TRUNCATE") == 0) {
        return IO_FS_TRUNCATE;
    }else if(strcmp(instruccion, "IO_FS_TRUNCATE") == 0) {
        return IO_FS_TRUNCATE;
    }else if(strcmp(instruccion, "IO_FD_WRITE") == 0) {
        return IO_FD_WRITE;
    }else if(strcmp(instruccion, "IO_GEN_SLEEP") == 0) {
        return IO_GEN_SLEEP;
    }else if(strcmp(instruccion, "IO_FS_READ") == 0) {
        return IO_FS_READ;
    }else if(strcmp(instruccion, "EXIT") == 0) {
        return EXIT;    
    }else{
        log_error(logger, "Error en buscar la instruccion");
        return 0;
    }
}

