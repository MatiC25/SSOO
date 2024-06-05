#include "instrucciones.h"

void send_instruccion(int socket_cliente, char *instruccion, t_list *parametros) {
    t_tipo_instruccion tipo_instruccion = obtener_tipo_instruccion(instruccion);
    t_paquete *paquete = crear_paquete(tipo_instruccion);

    agregar_a_paquete_lista_string(paquete, parametros);
    eliminar_paquete(paquete);
}


// t_instruccion* recv_instruccion(int socket_cliente) {
//     t_instruccion* instruccion = inicializar_instruccion();
//     instruccion->tipo = recibir_operacion(socket_cliente);
//     instruccion->parametros = recv_list(socket_cliente);
//     return instruccion;
// }

int recv_pagina(int socket_cliente){
    int tamanio;
    int marco;
    recv(socket_cliente,tamanio,sizeof(int),MSG_WAITALL);
    void* buffer = recibir_buffer(&tamanio, socket_cliente);
    memcpy(&marco,buffer,sizeof(int));
    free(buffer);
    return marco;
}


t_instruccion* recv_instruccion(int socket_cliente){
    int tamanio;
    t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    recv(socket_cliente,&tamanio,sizeof(int),MSG_WAITALL);

    void* buffer = recibir_buffer(&tamanio,socket_cliente);

    memcpy(&instruccion, buffer, sizeof(t_instruccion));
    
    
    free(buffer);

 
   
   
    // t_instruccion *instruccion = malloc(sizeof(t_instruccion));
    
    // if (recv(socket_cliente,&instruccion->opcode,instruccion->long_opcode,MSG_WAITALL)<= 0 ){
    //     free(instruccion);
    //     return NULL;
    // }
    // if (recv(socket_cliente,&instruccion->parametro1,instruccion->long_par1,MSG_WAITALL)<= 0 ){
    //     free(instruccion);
    //     return NULL;
    // }
    // if (recv(socket_cliente,&instruccion->parametro2,instruccion->long_par2,MSG_WAITALL)<= 0 ){
    //     free(instruccion);
    //     return NULL;
    // }
    // if (recv(socket_cliente,&instruccion->parametro3,instruccion->long_par3,MSG_WAITALL)<= 0 ){
    //     free(instruccion);
    //     return NULL;
    // }
    // if (recv(socket_cliente,&instruccion->parametro4,instruccion->long_par4,MSG_WAITALL)<= 0 ){
    //     free(instruccion);
    //     return NULL;
    // }
    return instruccion;
}


void solicitar_instruccion(int socket_server, int PID, int program_counter) {
    t_paquete *paquete = crear_paquete(INSTRUCCION);
    agregar_a_paquete(paquete, &PID, sizeof(int));
    agregar_a_paquete(paquete, &program_counter, sizeof(int));
    enviar_paquete(paquete, socket_server);
    eliminar_paquete(paquete);
}

t_tipo_instruccion obtener_tipo_instruccion(char *instruccion) {
    if(strcmp(instruccion, "SET") == 0) {
        return SET;
    } else if(strcmp(instruccion, " MOVE_IN") == 0) {
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
    }
    return 0;
}
