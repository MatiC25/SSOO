#include "init_instrucciones.h"

// se encarga de interpretar el código de operación (opcode) de la instrucción obtenida y 
// llamar a la función correspondiente para ejecutar la acción asociada a ese opcode




void ciclo_instrucciones(int socket){

    fetchInstrucciones(socket,);
    decode();
    execute();

    return;
}

// char* registros_string(t_registro_cpu registros){
//     switch (registros) {
//     case PC:
//         return "PC"
//         break;
//     case AX:
//         return "AX"
//         break;    
//     case BX:
//         return "BX"
//         break;
//     case CX:
//         return "CX"
//         break;
//     case DX:
//         return "DX"
//         break;
//     case EAX:
//         return "EAX"
//         break;
//     case EBX:
//         return "EBX"
//         break;
//     case ECX:
//         return "ECX"
//         break;
//     case EDX:
//         return "EDX"
//         break;
//     default
//         return ""
//         break;
//     }
// }


void buscar_registroValor(char* registro, uint32_t valor){
    
    if (strcmp(registro,"PC") == 0) {    
        pcb_cpu->registros->PC = valor;
    }
    if (strcmp(registro,"AX") == 0) {    
        pcb_cpu->registros->AX = valor;
    }
    else if(strcmp(registro,"BX") == 0) {
        pcb_cpu->registros->BX = valor;
    }
    else if(strcmp(registro,"CX") == 0) {
        pcb_cpu->registros->CX = valor;
    }
    else if(strcmp(registro,"DX") == 0) {
        pcb_cpu->registros->DX = valor;
    }
    else if(strcmp(registro,"EAX") == 0) {
        pcb_cpu->registros->EAX = valor;
    }
    else if(strcmp(registro,"EBX") == 0) {
        pcb_cpu->registros->EBX = valor;
    }
    else if(strcmp(registro,"ECX") == 0) {
        pcb_cpu->registros->ECX = valor;  
    }
    else if(strcmp(registro->EDX,"EDX") == 0) {
        pcb_cpu->registros->EDX = valor;
    }
}   


// char* buscar_registroValor(t_registro_cpu* registro, uint32_t valor {
//     switch (registro)
//         {//CREAR DICCIONARIO 
//         case PC: 
//             return memcpy(registro , valor , 4);
//         case AX:
//             return memcpy(registro , valor , 1);    
//         case BX:
//             return memcpy(registro , valor , 1);
//         case CX:
//             return memcpy(registro , valor , 1);
//         case DX:
//             return memcpy(registro , valor , 1);
//         case EAX:
//             return memcpy(registro , valor , 4);
//         case EBX:
//             return memcpy(registro , valor , 4);
//         case ECX:
//             return memcpy(registro , valor , 4);
//         case EDX:
//             return memcpy(registro , valor , 4);
//         default
//             return ""
//         }
//     }


void decode(t_instruccion* instruccion) { // y execute
    int tamanio = 0;
    int recurso = 0;

    op_code nombreinstruccion = instruccion->nombre; //Decode

    //execuite
    switch (instruccion -> nombre) {
        case EXIT:
            return;
        case SET:
            set(); //cabiar todo el resto!!! poner modo lista TODO 
            break;
        case SUM:
            funcion_SUM_cpu();
            break;
        case SUB:
            funcion_SUB_cpu();
            break;           
        case JNZ:
            Function_JNZ_cpu();
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

void inicializarOperandos_registro(t_instruccion *operando)
{
    operando -> nombreArchivo = NULL;
    operando -> valor = NULL;
}


t_instruccion* fetchInstrucciones(int socket) {

    t_instruccion* instruccion_memoria = pedir_instruccion_a_memoria(t_pcb_cpu->program_counter, t_pcb_cpu->pid, socket);
    t_pcb_cpu -> program_counter++;

    return instruccion_memoria;
 }




t_instruccion* pedir_instruccion_a_memoria(int program_Counter, int socket) {
    char* peticion = "Hola me pasas instrucicones :D !!"
    enviar_mensaje(peticion, socket);
    
    paquete(socket, program_Counter);
    
    for (size_t i = 0; i < 5/*(maximo)*/; i++) {
        list_get(litsta,i)
    }
    return recibir_instruccion();
}


void funcion_SET_cpu(t_registro_cpu* registro,char* valor) {
   // char* registro_convertido = registros_string(registro);
    uint32_t valor_convertido = atoi(valor); //No lo paso a char* lo mantengo en uint32_t
    buscar_registroValor(registro,valor_convertido);
}

uint32_t funcion_SUM_cpu(t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroOrigen) {
    uint32_t t_registro_suma;
    return t_registro_suma = t_registroDireccion + t_registroOrigen; //Esto se que que esta mal
}


void funcion_SUB_cpu(t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroOrigen) {

}

void funcion_JNZ_cpu(t_registro_cpu* t_registro,t_instruccion* t_instrucciones) {

}

void function_MOV_IN_cpu(t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroDato) {
    
}

void function_MOV_OUT_cpu(int tamanio) {

}

void function_COPY_STRING_cpu(int tamanio) {

}

void function_WAIT_cpu(int recurso) {

}

void function_SINGAL_cpu(int recurso) { 

}

void function_IO_STDIN_READ_cpu(/*INTERFAZ*/, t_registro_cpu* t_registroDireccion,t_registro_cpu* t_registroTamanio) {

}

void function_IO_STDOUT_WRITE_cpu(/*INTERFAZ*/,t_registro_cpu* t_registroDireccion, t_registro_cpu* t_registroTamanio) {

}

void function_IO_FS_CREATE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo) {

}

void function_IO_FS_DELETE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo) {

}

void function_IO_FS_TRUNCATE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo){

}
void function_IO_FD_WRITE_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo,t_registro_cpu* t_registroDireccion,t_registro_cpu* registroTamanio,t_registro_cpu registroPuntero) {

}

void function_IO_FS_READ_cpu(/*INTERFAZ*/, t_registro_cpu* nombreArchivo,t_registro_cpu* t_registroDireccion,t_registro_cpu* registroTamanio,t_registro_cpu registroPuntero) {

}

void librerar_pcb() {
    //hacer
}