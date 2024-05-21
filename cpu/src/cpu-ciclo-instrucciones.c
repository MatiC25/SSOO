#include "cpu-ciclo-instrucciones.h"
//BRANCH DE FEDE juntado con branch leo nico
t_pcb_cpu* pcb;
int seguir_ejecutando;

void iniciar_ciclo_de_ejecucion(int socket_server) {

   int socket_cliente = esperar_cliente("CPU", socket_server);

    while(1) {
        op_code codigo_operacion = recibir_operacion(socket_cliente);

        switch(codigo_operacion) {
            case MENSAJE:
				recibir_mensaje(socket_cliente);
				break;
             case RECIBIR_PROCESO:
                ejecutar_ciclo_instrucciones(socket_cliente, socket_server);
                break;
            case PAQUETE:
                break;
            case SOLICITAR_INSTRUCCION:
                break;
       }
    }
}

void ejecutar_ciclo_instrucciones(int socket_cliente, int socket_server) {
    
    recibir_pcb_a_kernel(socket_cliente); //Del kernel
     
        fecth(socket_server);
        ejecutar_instruccion(socket_cliente);
}   

 void recibir_pcb_a_kernel(int socket_cliente){
    int tamanio;

    recv(socket_cliente,&tamanio,sizeof(int),MSG_WAITALL);

    void* buffer = recibir_buffer(&tamanio,socket_cliente);

    memcpy(&pcb->PID,buffer,sizeof(int));
    memcpy(&pcb->program_counter,buffer + sizeof(int),sizeof(int));
    
    free(buffer);

 }

void fecth(int socket_server){
    int PID = pcb->PID;
    int program_counter = pcb->program_counter++;
    solicitar_instruccion(socket_server,PID, program_counter);
    log_info(logger,"Fetch Instruccion: PID: %d - FETCH -Programn Counter: %d",PID,program_counter);
}

void* obtener_registro (char *registro) {
    if(strcmp(registro, "AX") == 0) {
        return &(pcb->registro->AX);
    } else if(strcmp(registro, "BX") == 0) {
        return &(pcb->registro->BX);
    } else if(strcmp(registro, "CX") == 0) {
        return &(pcb->registro->CX);
    } else if(strcmp(registro, "DX") == 0) {
        return &(pcb->registro->DX);
    } else if(strcmp(registro, "PC") == 0) {
        return &(pcb->registro->PC);
    } else if(strcmp(registro, "EAX") == 0) {
        return &(pcb->registro->EAX);
    } else if(strcmp(registro, "EBX") == 0) {
        return &(pcb->registro->EBX);
    }else if(strcmp(registro, "ECX") == 0) {
        return &(pcb->registro->ECX);
    }else if(strcmp(registro, "EDX") == 0) {
        return &(pcb->registro->EDX);
    }else if(strcmp(registro, "SI") == 0) {
        return &(pcb->registro->SI);
    }else if(strcmp(registro, "DI") == 0) {
        return &(pcb->registro->DI);
    }else {
        return NULL;
    }
}
void operar_registros(void* registro_destino, void* registro_origen, char* registro, char* operacion, int valor){
    if (strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0|| strcmp(registro, "CX") == 0|| strcmp(registro, "DX") == 0){
        if (strcmp(operacion, "+")){
            *(uint8_t*)registro_destino += *(uint8_t*) registro_origen;
        }else if (strcmp(operacion, "-")){
            *(uint8_t*)registro_destino -= *(uint8_t*) registro_origen;
        }else if(strcmp(operacion, "set")){
            *(uint32_t*)registro_destino = valor;
        }else{  
            log_error(logger, "los registros no son de 8 bits");
        }
    }else if(strcmp(registro, "PC") == 0 || strcmp(registro, "EAX") == 0|| strcmp(registro, "EBX") == 0 || strcmp(registro, "ECX") == 0 || strcmp(registro, "EDX") == 0 || strcmp(registro, "SI") == 0|| strcmp(registro, "DI") == 0){
        if (strcmp(operacion, "+")){
            *(uint32_t*)registro_destino += *(uint32_t*) registro_origen;
        }else if (strcmp(operacion, "-")){
            *(uint32_t*)registro_destino -= *(uint32_t*) registro_origen;
        }else if(strcmp(operacion, "set")){
            *(uint32_t*)registro_destino = valor;
        }else{
            log_error(logger, "los registros no son de 32 bits");
        }
    }else{
        log_error(logger, "Registros desconocidos");
    }
    
}




void ejecutar_instruccion(int socket_cliente) {
    t_instruccion *instruccion = recv_instruccion(socket_cliente);
    //t_tipo_instruccion tipo_instruccion = list_get(instruccion->parametros ,0); //Decode
      t_tipo_instruccion tipo_instruccion = instruccion->opcode; //decode
        switch (tipo_instruccion)
        {
        case EXIT:
            return; 
            break;
        case SET:
            ejecutar_set(&instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SUM:
            ejecutar_sum(&instruccion->parametro1,&instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SUB:
            ejecutar_sub(&instruccion->parametro1,&instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;           
        case JNZ:
            ejecutar_JNZ(&instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_GEN_SLEEP:
            //ejecutar_IO_GEN_SLEEP(/*INTERFAZ*/,/*UNIDAD DE TRABAJO*/); //sleep
           log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case MOVE_IN:
           //ejecutar_MOV_IN(t_instrucciones->registroDireccion, t_instrucciones->registroDatos);
           log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case MOV_OUT:
           //ejecutar_MOV_OUT(t_instrucciones->registroDireccion, t_instrucciones->registroDatos);
           log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case RESIZE:
            //ejecutar_MOV_OUT(tamanio);
           log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case COPY_STRING:
           // ejecutar_COPY_STRING(tamanio);
           log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case WAIT:
            //ejecutar_WAIT(recurso);
           log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SINGAL:
            //ejecutar_SINGAL(recurso);
           log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_STDIN_READ:
            //ejecutar_IO_STDIN_READ(/*INTERFAZ*/, t_instrucciones->registroDireccion,t_instrucciones->registroTamanio);
          log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
        case IO_STDOUT_WRITE:
            //ejecutar_IO_STDOUT_WRITE(/*INTERFAZ*/,t_instrucciones->registroDireccion, t_instrucciones->registroTamanio);
           log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_CREATE:
            //ejecutar_IO_FS_CREATE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
          log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_DELETE:
            //ejecutar_IO_FS_DELETE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
           log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_TRUNCATE:
            //ejecutar_IO_FS_TRUNCATE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
           log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FD_WRITE:
            //ejecutar_IO_FD_WRITE(/*INTERFAZ*/, t_instrucciones->nombreArchivo,t_instrucciones->registroDireccion,t_instrucciones->registroTamanio,t_instrucciones->registroPuntero);
           log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;       
         case IO_FS_READ:
            //ejecutar_IO_FS_READ(/*INTERFAZ*/, t_instrucciones->nombreArchivo,t_instrucciones->registroDireccion,t_instrucciones->registroTamanio,t_instrucciones->registroPuntero);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c",pcb->PID,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;          
        }
}

void ejecutar_set(char* registro, char valor){
    void* reg = obtener_registro(registro);
    if(reg != NULL){
        operar_registros(reg,NULL,registro,"set",atoi(&valor));
    }else{
        log_error(logger,"Error al obtener el SET");
    }
}

void ejecutar_sum(char* registro_origen_char, char* registro_desitino_char) {
    void* registro_origen = obtener_registro(registro_origen_char);
    void* registro_destino = obtener_registro(registro_desitino_char);

    if (registro_origen != NULL && registro_destino != NULL)
    {
        operar_registros(registro_destino,registro_origen,registro_origen_char,"+",0);
    }else{
        log_error(logger,"Error al obtener el SUM");
    }
}


void ejecutar_sub(char* registro_origen_char, char* registro_desitino_char){
    void* registro_origen = obtener_registro(registro_origen_char);
    void* registro_destino = obtener_registro(registro_desitino_char);

       if (registro_origen != NULL && registro_destino != NULL)
    {
        operar_registros(registro_destino,registro_origen,registro_origen_char,"-", 0);
    }else{
        log_error(logger,"Error al obtener el SUB");
    }
}

void ejecutar_JNZ(char* registro, char valor){
    void* reg = obtener_registro(registro);
    if (reg == 0)
    {
        pcb->program_counter += atoi(&valor);
    }else{
        log_error(logger,"Error al obtener el JNZ");
    }
}

//     sleep();
// }
