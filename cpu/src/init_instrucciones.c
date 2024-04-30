#include "init_instrucciones.h"

// se encarga de interpretar el código de operación (opcode) de la instrucción obtenida y 
// llamar a la función correspondiente para ejecutar la acción asociada a ese opcode
void decode(t_instruccion* t_instrucciones, char* registro)   {
    int tamanio = 0;
    int recurso = 0;


    op_code code = t_instrucciones->instruccion; 

    if (t_instrucciones->instruccion == EXIT)
    {
        return
    }

    while (t_instrucciones->instruccion != EXIT)
    {
        switch (t_instrucciones->instruccion)
        {
        case SET:
            funcion_SET_cpu(registro, t_instrucciones->valor);
            break;
        case SUM:
            funcion_SUM_cpu(t_instrucciones->registroDireccion, t_instrucciones->registroOrigen);
            break;
        case SUB:
            funcion_SUB_cpu(t_instrucciones->registroDireccion, t_instrucciones->registroOrigen);
            break;           
        case JNZ:
            Function_JNZ_cpu(t_instrucciones->registro, t_instrucciones->instruccion);
            break;
        // case IO_GEN_SLEEP:
        //     Funcion_IO_GEN_SLEEP_cpu(/*INTERFAZ*/,/*UNIDAD DE TRABAJO*/);
        //     break;
        // case MOVE_IN:
        //     Function_MOV_IN_cpu(t_instrucciones->registroDireccion, t_instrucciones->registroDatos);
        //     break;
        // case RESIZE:
        //     Function_MOV_OUT_cpu(tamanio);
        //     break;
        // case COPY_STRING:
        //     Function_COPY_STRING_cpu(tamanio);
        //     break;
        // case WAIT:
        //     Function_WAIT_cpu(recurso);
        //     break;
        // case SINGAL:
        //     Function_SINGAL_cpu(recurso);
        //     break;
        // case IO_STDIN_READ:
        //     Function_IO_STDIN_READ_cpu(/*INTERFAZ*/, t_instrucciones->registroDireccion,t_instrucciones->registroTamanio);
        //     break;
        // case IO_STDOUT_WRITE:
        //     Function_IO_STDOUT_WRITE_cpu(/*INTERFAZ*/,t_instrucciones->registroDireccion, t_instrucciones->registroTamanio);
        //     break;
        // case IO_FS_CREATE:
        //     Function_IO_FS_CREATE_cpu(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
        //     break;
        // case IO_FS_DELETE:
        //     Function_IO_FS_DELETE_cpu(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
        //     break;
        // case IO_FS_TRUNCATE:
        //     Function_IO_FS_TRUNCATE_cpu(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
        //     break;
        // case IO_FD_WRITE:
        //     Function_IO_FD_WRITE_cpu(/*INTERFAZ*/, t_instrucciones->nombreArchivo,t_instrucciones->registroDireccion,t_instrucciones->registroTamanio,t_instrucciones->registroPuntero);
        //     break;       
        //  case IO_FS_READ:
        //     Function_IO_FS_READ_cpu(/*INTERFAZ*/, t_instrucciones->nombreArchivo,t_instrucciones->registroDireccion,t_instrucciones->registroTamanio,t_instrucciones->registroPuntero);
        //     break;          
        }
    }
    return
}    

void inicializarOperandos_registro(t_instruccion *operando)
{
    operando -> nombreArchivo = NULL;
    operando -> valor = NULL;
}

void incrementoDePC_cpu(t_pcb *registro)
{   
    registro->registros->PC ++;
}


void fetchInstrucciones(int socket_cpu) {

    pcb_cpu* contexto_actual = rcv_contexto_ejecucion(socket_cpu);
    int pid_actual = contexto_actual->pid;
    //SEMAFORO¿
    while(true)
    {
        //LOG MINIMO
        log_info(logger, "Fetch Instrucción: PID: %d - FETCH - Program Counter: %d", contexto_actual->pid, contexto_actual->program_counter);

        





    }
    
    
    // Incrementar el Program Counter para apuntar a la siguiente instrucción
    incrementoDePC_cpu(registro_cpu);

    
}

void funcion_SET_cpu(char* valor,char* registro){
    uint32_t valor_convertido = atoi(valor); //No lo paso a char* lo mantengo en uint32_t
    buscar_registroValor(registro,valor_convertido);
}

void buscar_registroValor(char* registro, uint32_t valor){
    
    if (strcmp(registro,"PC") == 0)
    {    
        pcb_cpu->registros->PC = valor;
    }
    if (strcmp(registro,"AX") == 0)
    {    
        pcb_cpu->registros->AX = valor;
    }
    else if(strcmp(registro,"BX") == 0)
    {
        pcb_cpu->registros->BX = valor;
    }
    else if(strcmp(registro,"CX") == 0)
    {
        pcb_cpu->registros->CX = valor;
    }
    else if(strcmp(registro,"DX") == 0)
    {
        pcb_cpu->registros->DX = valor;
    }
    else if(strcmp(registro,"EAX") == 0)
    {
        pcb_cpu->registros->EAX = valor;
    }
    else if(strcmp(registro,"EBX") == 0)
    {
        pcb_cpu->registros->EBX = valor;
    }
    else if(strcmp(registro,"ECX") == 0)
    {
        pcb_cpu->registros->ECX = valor;  
    }
    else if(strcmp(registro->EDX,"EDX") == 0)
    {
        pcb_cpu->registros->EDX = valor;
    }
}   

void funcion_SUM_cpu(t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroOrigen){
    t_registro_cpu
}


void funcion_SUB_cpu(t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroOrigen){

}

void funcion_JNZ_cpu(t_registro_cpu* t_registro,t_instruccion* t_instrucciones){

}

void function_MOV_IN_cpu(t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroDato){
    
}

void function_MOV_OUT_cpu(int tamanio){

}

void function_COPY_STRING_cpu(int tamanio){

}

void function_WAIT_cpu(int recurso){

}

void function_SINGAL_cpu(int recurso){

}

void function_IO_STDIN_READ_cpu(/*INTERFAZ*/, t_registro_cpu* t_registroDireccion,t_registro_cpu* t_registroTamanio){

}

void function_IO_STDOUT_WRITE_cpu(/*INTERFAZ*/,t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroTamanio){

}

void function_IO_FS_CREATE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo){

}

void function_IO_FS_DELETE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo){

}

void function_IO_FS_TRUNCATE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo){

}
void function_IO_FD_WRITE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo,t_registro_cpu* t_registroDireccion,t_registro_cpu* registroTamanio,t_registro_cpu registroPuntero){

}

void function_IO_FS_READ_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo,t_registro_cpu* t_registroDireccion,t_registro_cpu* registroTamanio,t_registro_cpu registroPuntero){

}


