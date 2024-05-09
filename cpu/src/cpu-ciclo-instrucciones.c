#include "cpu-ciclo-instrucciones.h"
//BRANCH DE FEDE juntado con branch leo nico

int seguir_ejecutando;

void iniciar_ciclo_de_ejecucion(int socket_server) {
   int socket_cliente = esperar_cliente("MEMORIA", socket_server);

    while(1) {
        op_code codigo_operacion = recibir_operacion(socket_cliente);

        switch(codigo_operacion) {
            case MENSAJE:
				recibir_mensaje(socket_cliente);
				break;
            case EJECUTAR_INSTRUCCIONES:
                ejecutar_ciclo_instrucciones(socket_cliente, socket_server);
                break;
       }
    }
}

void ejecutar_ciclo_instrucciones(int socket_cliente, int socket_server) {
    t_instruccion *instruccion;
    seguir_ejecutando = 0;
    t_pcb = recibir_pcb_a_kernel(socket_cliente); //Del kernel
    
    while(!seguir_ejecutando) {
        fecth(socket_server);
        ejecutar_instruccion(socket_cliente);
    }
}   

// t_pcb* recibir_pcb_a_kernel(int socket_cliente){
//      codear como me llega la PCB del kernel
//        return pcb_cpu;
// }


void fecth(int socket_server){
    int PID = t_pcb->pid;
    int program_counter = t_pcb->program_counter++
    solicitar_instruccion(socket_server,PID, program_counter);
    log_info(logger,"Fetch Instruccion: PID: %d - FETCH -Programn Counter: %d",PID,program_counter);
}

t_registro_cpu* obtener_registro (char *registro) {
    if(strcmp(registro, "AX") == 0) {
        return &(t_pcb->registros->AX)
    } else if(strcmp(registro, "BX") == 0) {
        return &(t_pcb->registros->BX);
    } else if(strcmp(registro, "CX") == 0) {
        return &(t_pcb->registros->CX);
    } else if(strcmp(registro, "DX") == 0) {
        return &(t_pcb->registros->DX);
    } else if(strcmp(registro, "PC") == 0) {
        return &(t_pcb->registros->PC);
    } else if(strcmp(registro, "EAX") == 0) {
        return &(t_pcb->registros->EAX);
    } else if(strcmp(registro, "EBX") == 0) {
        return &(t_pcb->registros->EBX);
    }else if(strcmp(registro, "ECX") == 0) {
        return &(t_pcb->registros->ECX);
    }else if(strcmp(registro, "EDX") == 0) {
        return &(t_pcb->registros->EDX);
    }else if(strcmp(registro, "SI") == 0) {
        return &(t_pcb->registros->SI);
    }else if(strcmp(registro, "DI") == 0) {
        return &(t_pcb->registros->DI);
    }else {
        return NULL;
    }
}


void ejecutar_instruccion(int socket_cliente) {
    t_instruccion *instruccion = recv_instruccion(socket_cliente);
    //t_tipo_instruccion tipo_instruccion = list_get(instruccion->parametros ,0); //Decode
      t_tipo_instruccion tipo_instruccion = instruccion->opcode; //decode
        switch (tipo_instruccion)
        {
        case SET:
            ejecutar_set(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -%s %s",instruccion->long_opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SUM:
            ejecutar_sum(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -%s %s",instruccion->long_opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SUB:
            ejecutar_sub(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -%s %s",instruccion->long_opcode,instruccion->parametro1,instruccion->parametro2);
            break;           
        case JNZ:
            ejecutar_JNZ(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -%s %s",instruccion->long_opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        // case IO_GEN_SLEEP:
        //     ejecutar_IO_GEN_SLEEP(/*INTERFAZ*/,/*UNIDAD DE TRABAJO*/); //sleep
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;
        // case MOVE_IN:
        //    ejecutar_MOV_IN(t_instrucciones->registroDireccion, t_instrucciones->registroDatos);
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;
        // case RESIZE:
        //     ejecutar_MOV_OUT(tamanio);
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;
        // case COPY_STRING:
        //     ejecutar_COPY_STRING(tamanio);
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;
        // case WAIT:
        //     ejecutar_WAIT(recurso);
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;
        // case SINGAL:
        //     ejecutar_SINGAL(recurso);
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;
        // case IO_STDIN_READ:
        //     ejecutar_IO_STDIN_READ(/*INTERFAZ*/, t_instrucciones->registroDireccion,t_instrucciones->registroTamanio);
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;
        // case IO_STDOUT_WRITE:
        //     ejecutar_IO_STDOUT_WRITE(/*INTERFAZ*/,t_instrucciones->registroDireccion, t_instrucciones->registroTamanio);
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;
        // case IO_FS_CREATE:
        //     ejecutar_IO_FS_CREATE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;
        // case IO_FS_DELETE:
        //     ejecutar_IO_FS_DELETE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;
        // case IO_FS_TRUNCATE:
        //     ejecutar_IO_FS_TRUNCATE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;
        // case IO_FD_WRITE:
        //     ejecutar_IO_FD_WRITE(/*INTERFAZ*/, t_instrucciones->nombreArchivo,t_instrucciones->registroDireccion,t_instrucciones->registroTamanio,t_instrucciones->registroPuntero);
        //    log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %s -Parametro1 Parametro2",tipo_instruccion); //cambiar cuando lo hagamos henerico
        //     break;       
        //  case IO_FS_READ:
        //     ejecutar_IO_FS_READ(/*INTERFAZ*/, t_instrucciones->nombreArchivo,t_instrucciones->registroDireccion,t_instrucciones->registroTamanio,t_instrucciones->registroPuntero);
        //     break;          
        }
}

void ejecutar_set(char* registro, char valor){
    t_registro_cpu* reg = obtener_registro(registro);
    if(reg != NULL){
        *reg = atoi(valor);
    }
}

void ejecutar_sum(char* registro_origen_char, char* registro_desitino_char) {
    t_registro_cpu* registro_origen = obtener_registro(t_pcb, registro_origen_char);
    t_registro_cpu* registro_destino = obtener_registro(t_pcb, registro_desitino_char);

    if (registro_origen != NULL && registro_destino != NULL)
    {
        *registro_destino += *registro_origen;
    }
}


void ejecutar_sub(char* registro_origen_char, char* registro_desitino_char){
    t_registro_cpu* registro_origen = obtener_registro(t_pcb, registro_origen_char);
    t_registro_cpu* registro_destino = obtener_registro(t_pcb, registro_desitino_char);

       if (registro_origen != NULL && registro_destino != NULL)
    {
        *registro_destino -= *registro_origen;
    }
}

void ejecutar_JNZ(char* registro, char valor){
    t_registro_cpu* reg = obtener_registro(t_pcb,registro);
    if (*reg == 0)
    {
        t_pcb->program_counter += atoi(valor);
    }
}

// void ejecutar_IO_GEN_SLEEP(){
//     sleep();
// }
