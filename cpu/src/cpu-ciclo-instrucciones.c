#include "cpu-ciclo-instrucciones.h"
//BRANCH DE FEDE juntado con branch leo nico
t_pcb_cpu* pcb;
//t_mmu_cpu* mmu;
int seguir_ejecutando;

void iniciar_ciclo_de_ejecucion(int socket_server) {

   int socket_cliente = esperar_cliente("DISPATCH", socket_server);
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
    
    rcv_contexto_ejecucion_cpu(socket_cliente);
     
        fecth(socket_server);
        ejecutar_instruccion(socket_cliente);
}   

t_pcb_cpu* rcv_contexto_ejecucion_cpu(int socket_cliente) {
    
    t_pcb_cpu* proceso = malloc(sizeof(t_pcb_cpu));
    if (proceso == NULL) {
        log_error(logger, "Error al asignar memoria para el proceso");
        return NULL;
    }

    proceso->registros = malloc(sizeof(t_registro_cpu));
    if (proceso->registros == NULL) {
        log_error(logger, "Error al asignar memoria para los registros del proceso");
        free(proceso);
        return NULL;
    }

    int size;
    int desplazamiento = 0;
    
    void* buffer = recibir_buffer(&size, socket_cliente);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
        free(proceso->registros);
        free(proceso);
        return NULL;
    }

    memcpy(&proceso->pid, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&proceso->program_counter, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&proceso->registros->PC, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->AX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&proceso->registros->BX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&proceso->registros->CX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&proceso->registros->DX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&proceso->registros->EAX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->EBX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->ECX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->EDX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->SI, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->DI, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    free(buffer);
    return proceso;
}


void fecth(int socket_server){
    int PID = pcb->pid;
    int program_counter = pcb->program_counter++;
    solicitar_instruccion(socket_server,PID, program_counter);
    log_info(logger,"Fetch Instruccion: PID: %d - FETCH -Programn Counter: %d",PID,program_counter);
}

void* obtener_registro (char *registro) {
    if(strcmp(registro, "AX") == 0) {
        return &(pcb->registros->AX);
    } else if(strcmp(registro, "BX") == 0) {
        return &(pcb->registros->BX);
    } else if(strcmp(registro, "CX") == 0) {
        return &(pcb->registros->CX);
    } else if(strcmp(registro, "DX") == 0) {
        return &(pcb->registros->DX);
    } else if(strcmp(registro, "PC") == 0) {
        return &(pcb->registros->PC);
    } else if(strcmp(registro, "EAX") == 0) {
        return &(pcb->registros->EAX);
    } else if(strcmp(registro, "EBX") == 0) {
        return &(pcb->registros->EBX);
    }else if(strcmp(registro, "ECX") == 0) {
        return &(pcb->registros->ECX);
    }else if(strcmp(registro, "EDX") == 0) {
        return &(pcb->registros->EDX);
    }else if(strcmp(registro, "SI") == 0) {
        return &(pcb->registros->SI);
    }else if(strcmp(registro, "DI") == 0) {
        return &(pcb->registros->DI);
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

void enviar_pcb_a_kernel(t_paquete* paquete_a_kernel){
    

    // Agregar información del PCB al paquete
    agregar_a_paquete(paquete_a_kernel, &pcb->pid, sizeof(int));
    agregar_a_paquete(paquete_a_kernel, &pcb->program_counter, sizeof(int));

    // Agregar los registros de la CPU al paquete individualmente
    agregar_a_paquete(paquete_a_kernel, &pcb->registros->PC, sizeof(uint32_t));
    agregar_a_paquete(paquete_a_kernel, &pcb->registros->AX, sizeof(uint8_t));
    agregar_a_paquete(paquete_a_kernel, &pcb->registros->BX, sizeof(uint8_t));
    agregar_a_paquete(paquete_a_kernel, &pcb->registros->CX, sizeof(uint8_t));
    agregar_a_paquete(paquete_a_kernel, &pcb->registros->DX, sizeof(uint8_t));
    agregar_a_paquete(paquete_a_kernel, &pcb->registros->EAX, sizeof(uint32_t));
    agregar_a_paquete(paquete_a_kernel, &pcb->registros->EBX, sizeof(uint32_t));
    agregar_a_paquete(paquete_a_kernel, &pcb->registros->ECX, sizeof(uint32_t));
    agregar_a_paquete(paquete_a_kernel, &pcb->registros->EDX, sizeof(uint32_t));
    agregar_a_paquete(paquete_a_kernel, &pcb->registros->SI, sizeof(uint32_t));
    agregar_a_paquete(paquete_a_kernel, &pcb->registros->DI, sizeof(uint32_t));

    // Enviar el paquete a la CPU
    enviar_paquete(paquete_a_kernel,config_cpu->SOCKET_DISPATCH);

    // Liberar recursos del paquete
    eliminar_paquete(paquete_a_kernel);
}

// int num_pagina(int direccion_logica, int tamano_pagina){

//     return floor(direccion_logica / tamano_pagina);
// }

// int desplazamiento(int direccion_logica, int numero_pagina, int tamano_pagina){

//     return (direccion_logica - numero_pagina * tamano_pagina);
// }



void ejecutar_instruccion(int socket_cliente) {
    t_instruccion *instruccion = recv_instruccion(socket_cliente);
      t_tipo_instruccion tipo_instruccion = obtener_tipo_instruccion(&instruccion->opcode); //decode
        switch (tipo_instruccion)
        {
        case EXIT:
            t_paquete* paquete_a_kernel = crear_paquete(EXIT);
            enviar_pcb_a_kernel(paquete_a_kernel);
            return; 
            break;
        case SET:
            ejecutar_set(&instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SUM:
            ejecutar_sum(&instruccion->parametro1,&instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SUB:
            ejecutar_sub(&instruccion->parametro1,&instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;           
        case JNZ:
            ejecutar_JNZ(&instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_GEN_SLEEP:
           ejecutar_IO_GEN_SLEEP(&instruccion->parametro1,&instruccion->parametro2);
           //Tipo desalojo IO
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case MOVE_IN:
           //ejecutar_MOV_IN(t_instrucciones->registroDireccion, t_instrucciones->registroDatos);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case MOV_OUT:
           //ejecutar_MOV_OUT(t_instrucciones->registroDireccion, t_instrucciones->registroDatos);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case RESIZE:
            //ejecutar_MOV_OUT(tamanio);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case COPY_STRING:
           // ejecutar_COPY_STRING(tamanio);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case WAIT:
            //ejecutar_WAIT(recurso);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SIGNAL:
            //ejecutar_SINGAL(recurso);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_STDIN_READ:
            //ejecutar_IO_STDIN_READ(/*INTERFAZ*/, t_instrucciones->registroDireccion,t_instrucciones->registroTamanio);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
        case IO_STDOUT_WRITE:
            //ejecutar_IO_STDOUT_WRITE(/*INTERFAZ*/,t_instrucciones->registroDireccion, t_instrucciones->registroTamanio);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_CREATE:
            //ejecutar_IO_FS_CREATE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_DELETE:
            //ejecutar_IO_FS_DELETE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_TRUNCATE:
            //ejecutar_IO_FS_TRUNCATE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FD_WRITE:
            //ejecutar_IO_FD_WRITE(/*INTERFAZ*/, t_instrucciones->nombreArchivo,t_instrucciones->registroDireccion,t_instrucciones->registroTamanio,t_instrucciones->registroPuntero);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;       
         case IO_FS_READ:
            //ejecutar_IO_FS_READ(/*INTERFAZ*/, t_instrucciones->nombreArchivo,t_instrucciones->registroDireccion,t_instrucciones->registroTamanio,t_instrucciones->registroPuntero);
            log_info(logger,"Instruccion Ejecutada: PID: %d - Ejecutando: %c -%c %c", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;          
        }
}

void ejecutar_set (char* registro, char valor){
    void* reg = obtener_registro(registro);
    if(reg != NULL){
        operar_registros(reg,NULL,registro,"set",atoi(&valor));
    }else{
        log_error(logger,"Error al obtener el SET");
    }

    if (atomic_load(&interrupt_flag) == 1){
    //Se recibio una interrupcion
    t_paquete* paquete_a_kernel = crear_paquete(FINQUANTUM);
    atomic_store(&interrupt_flag,0); 
    enviar_pcb_a_kernel(paquete_a_kernel);
    }

}

void ejecutar_sum (char* registro_origen_char, char* registro_desitino_char) {
    void* registro_origen = obtener_registro(registro_origen_char);
    void* registro_destino = obtener_registro(registro_desitino_char);

    if (registro_origen != NULL && registro_destino != NULL)
    {
        operar_registros(registro_destino,registro_origen,registro_origen_char, "+" ,0);
    }else{
        log_error(logger,"Error al obtener el SUM");
    }

    if (atomic_load(&interrupt_flag) == 1){
    //Se recibio una interrupcion
    t_paquete* paquete_a_kernel = crear_paquete(FINQUANTUM);
    atomic_store(&interrupt_flag,0);
    enviar_pcb_a_kernel(paquete_a_kernel);
    }   
}


void ejecutar_sub (char* registro_origen_char, char* registro_desitino_char){
    void* registro_origen = obtener_registro(registro_origen_char);
    void* registro_destino = obtener_registro(registro_desitino_char);

       if (registro_origen != NULL && registro_destino != NULL)
    {
        operar_registros(registro_destino,registro_origen,registro_origen_char, "-" , 0);
    }else{
        log_error(logger,"Error al obtener el SUB");
    }
    if (atomic_load(&interrupt_flag) == 1){
    //Se recibio una interrupcion
    t_paquete* paquete_a_kernel = crear_paquete(FINQUANTUM);
    atomic_store(&interrupt_flag,0);
    enviar_pcb_a_kernel(paquete_a_kernel);
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
    if (atomic_load(&interrupt_flag) == 1){
    //Se recibio una interrupcion
    t_paquete* paquete_a_kernel = crear_paquete(FINQUANTUM);
    atomic_store(&interrupt_flag,0);
    enviar_pcb_a_kernel(paquete_a_kernel);
    }
}

void ejecutar_IO_GEN_SLEEP(char* interfazAUsar, char* tiempoDeTrabajo){
    t_paquete* paquete_a_kernel = crear_paquete(OPERACION_IO);
    agregar_a_paquete(paquete_a_kernel, &interfazAUsar, sizeof(char*));
    agregar_a_paquete(paquete_a_kernel, &tiempoDeTrabajo, sizeof(char*));

    enviar_pcb_a_kernel(paquete_a_kernel);
    // Enviar_Dato_Por_Dispacht
}

//Parte 2 min 47