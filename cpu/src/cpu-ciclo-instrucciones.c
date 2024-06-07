#include "cpu-ciclo-instrucciones.h"
//BRANCH DE FEDE juntado con branch leo nico
t_pcb_cpu* pcb;
int seguir_ejecutando;

void iniciar_ciclo_de_ejecucion(int socket_server ,int socket_cliente) {
    
    while(1) {
        op_code codigo_operacion = recibir_operacion(socket_cliente);

        switch(codigo_operacion) {
            case MENSAJE:
				recibir_mensaje(socket_cliente);
				break;
             case RECIBIR_PROCESO:
                ejecutar_ciclo_instrucciones(socket_cliente, socket_server);
                break;
            case HANDSHAKE:
            log_info(logger, "Handshake exitoso con Interrupt");
                break;
        default:
        log_error(logger, "Operacion desconocida");
       }
    }   
}

void ejecutar_ciclo_instrucciones(int socket_cliente, int socket_server) {
    
    rcv_contexto_ejecucion_cpu(socket_cliente);
    seguir_ciclo(socket_cliente, socket_server);
}   

void seguir_ciclo(){
    fecth(config_cpu->SOCKET_MEMORIA);
    ejecutar_instruccion(config_cpu->SOCKET_KERNEL);
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

int espacio_de_registro(char* registro){
    if (strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0|| strcmp(registro, "CX") == 0|| strcmp(registro, "DX") == 0){
        return 8; //en bits
    }else if (strcmp(registro, "PC") == 0 || strcmp(registro, "EAX") == 0|| strcmp(registro, "EBX") == 0 || strcmp(registro, "ECX") == 0 || strcmp(registro, "EDX") == 0 || strcmp(registro, "SI") == 0|| strcmp(registro, "DI") == 0){
        return 32; //en bits
    }else{
        log_error(logger,"Error al calulcar el tamanio del registro");
        return 0;
    }
}

void operar_con_registros(void* registro_destino, void* registro_origen, char* registro, char* operacion, int valor){
    if (strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0|| strcmp(registro, "CX") == 0|| strcmp(registro, "DX") == 0){
        if (strcmp(operacion, "+")){
            *(uint8_t*)registro_destino += *(uint8_t*) registro_origen;
        }else if (strcmp(operacion, "-")){
            if(*(uint8_t*)registro_origen <= *(uint8_t*)registro_destino){
                *(uint8_t*)registro_destino -= *(uint8_t*) registro_origen;
            }else{log_error(logger,"Los registros negativos no se guardaran correctamente");}
        }else if(strcmp(operacion, "set")){
            *(uint8_t*)registro_destino = valor;
        }else{  
            log_error(logger, "los registros no son de 8 bits");
        }
    }else if(strcmp(registro, "PC") == 0 || strcmp(registro, "EAX") == 0|| strcmp(registro, "EBX") == 0 || strcmp(registro, "ECX") == 0 || strcmp(registro, "EDX") == 0 || strcmp(registro, "SI") == 0|| strcmp(registro, "DI") == 0){
        if (strcmp(operacion, "+")){
            *(uint32_t*)registro_destino += *(uint32_t*) registro_origen;
        }else if (strcmp(operacion, "-")){
            if(*(uint32_t*)registro_origen <= *(uint32_t*)registro_destino){
            *(uint32_t*)registro_destino -= *(uint32_t*) registro_origen;
            }else{log_error(logger,"Los registros negativos no se guardaran correctamente");}
        }else if(strcmp(operacion, "set")){
            *(uint32_t*)registro_destino = valor;
        }else{
            log_error(logger, "los registros no son de 32 bits");
        }
    }else{
        log_error(logger, "Registros desconocidos");
    }
    
    
}

void tengoAlgunaInterrupcion(){
    if (atomic_load(&interrupt_flag) == 1){
    //Se recibio una interrupcion
    t_paquete* paquete_a_kernel = crear_paquete(FINQUANTUM);
    atomic_store(&interrupt_flag,0);
    enviar_pcb_a_kernel(paquete_a_kernel);
    }else{
        seguir_ciclo();
    }  
}

void ejecutar_instruccion(int socket_cliente) {
    t_instruccion *instruccion = recv_instruccion(socket_cliente);
      t_tipo_instruccion tipo_instruccion = obtener_tipo_instruccion(instruccion->opcode); //decode
        switch (tipo_instruccion)
        {
        case EXIT:
            t_paquete* paquete_a_kernel = crear_paquete(EXIT);
            enviar_pcb_a_kernel(paquete_a_kernel);
            return; 
            break;
        case SET:
            ejecutar_set(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SUM:
            ejecutar_sum(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SUB:
            ejecutar_sub(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;           
        case JNZ:
            ejecutar_JNZ(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_GEN_SLEEP:
           ejecutar_IO_GEN_SLEEP(instruccion->parametro1,instruccion->parametro2);
           //Tipo desalojo IO
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case MOVE_IN:
            ejecutar_MOV_IN(instruccion->parametro1, instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case MOV_OUT:
           ejecutar_MOV_OUT(instruccion->parametro1, instruccion->parametro2);
           log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case RESIZE:
            ejecutar_RESIZE(instruccion->parametro1);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s ", pcb->pid,instruccion->opcode,instruccion->parametro1);
            break;
        case COPY_STRING:
            ejecutar_COPY_STRING(instruccion->parametro1);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s ", pcb->pid,instruccion->opcode,instruccion->parametro1);
            break;
        case WAIT:
            ejecutar_WAIT(instruccion->parametro1);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s", pcb->pid,instruccion->opcode,instruccion->parametro1);
            break;
        case SIGNAL:
            ejecutar_SINGAL(instruccion->parametro1);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s", pcb->pid,instruccion->opcode,instruccion->parametro1);
            break;
        case IO_STDIN_READ:
            //ejecutar_IO_STDIN_READ(/*INTERFAZ*/, t_instrucciones->registroDireccion,t_instrucciones->registroTamanio);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
        case IO_STDOUT_WRITE:
            //ejecutar_IO_STDOUT_WRITE(/*INTERFAZ*/,t_instrucciones->registroDireccion, t_instrucciones->registroTamanio);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_CREATE:
            //ejecutar_IO_FS_CREATE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_DELETE:
            //ejecutar_IO_FS_DELETE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_TRUNCATE:
            //ejecutar_IO_FS_TRUNCATE(/*INTERFAZ*/, t_instrucciones->nombreArchivo);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FD_WRITE:
            //ejecutar_IO_FD_WRITE(/*INTERFAZ*/, t_instrucciones->nombreArchivo,t_instrucciones->registroDireccion,t_instrucciones->registroTamanio,t_instrucciones->registroPuntero);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;       
         case IO_FS_READ:
            //ejecutar_IO_FS_READ(/*INTERFAZ*/, t_instrucciones->nombreArchivo,t_instrucciones->registroDireccion,t_instrucciones->registroTamanio,t_instrucciones->registroPuntero);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        default:
        log_error(logger, "Operacion desconocida");          
        }
}

void ejecutar_set (char* registro, char* valor){
    void* reg = obtener_registro(registro);
    if(reg != NULL){
        operar_con_registros(reg,NULL,registro,"set",atoi(valor));
    }else{
        log_error(logger,"Error al obtener el SET");
    }
    tengoAlgunaInterrupcion();

}

void ejecutar_sum (char* registro_origen_char, char* registro_desitino_char) {
    void* registro_origen = obtener_registro(registro_origen_char);
    void* registro_destino = obtener_registro(registro_desitino_char);

    if (registro_origen != NULL && registro_destino != NULL)
    {
        operar_con_registros(registro_destino,registro_origen,registro_origen_char, "+" ,0);
    }else{
        log_error(logger,"Error al obtener el SUM");
    }
    tengoAlgunaInterrupcion();  
}


void ejecutar_sub (char* registro_origen_char, char* registro_desitino_char){
    void* registro_origen = obtener_registro(registro_origen_char);
    void* registro_destino = obtener_registro(registro_desitino_char);

       if (registro_origen != NULL && registro_destino != NULL)
    {
        operar_con_registros(registro_destino,registro_origen,registro_origen_char, "-" , 0);
    }else{
        log_error(logger,"Error al obtener el SUB");
    }
    tengoAlgunaInterrupcion();
}

void ejecutar_JNZ(char* registro, char* valor){
    void* reg = obtener_registro(registro);
    if (reg == 0)
    {
        pcb->program_counter += atoi(valor);
    }else{
        log_error(logger,"Error al obtener el JNZ");
    }
    tengoAlgunaInterrupcion();
}

void ejecutar_IO_GEN_SLEEP(char* interfazAUsar, char* tiempoDeTrabajo){
    t_paquete* paquete_a_kernel = crear_paquete(OPERACION_IO);
    agregar_a_paquete(paquete_a_kernel, &interfazAUsar, sizeof(char*));
    agregar_a_paquete(paquete_a_kernel, &tiempoDeTrabajo, sizeof(char*));

    enviar_pcb_a_kernel(paquete_a_kernel);
    // Enviar_Dato_Por_Dispacht
}

void ejecutar_MOV_IN(char* registro_Datos, char* registro_Direccion){
    void* reg_Direccion = obtener_registro(registro_Direccion);
    void* reg_Datos   = obtener_registro(registro_Datos);
    uint32_t direcLogi = *(uint32_t*)reg_Direccion;
    int direccionLogica = (int)direcLogi;
    t_mmu_cpu* mmu_mov_in = traducirDireccion(direccionLogica);
    int tamanio_registro = espacio_de_registro(registro_Datos);

    //Noc como hacer la verificacion ver despues 
    enviar_a_leer_memoria(pcb->pid,mmu_mov_in->direccionFIsica, tamanio_registro); //Memoria me dijo que no hace falta el PID 
    int valor = recv_leer_memoria();

    operar_con_registros(reg_Datos,NULL,registro_Datos,"set",valor);
    free(mmu_mov_in);
    void tengoAlgunaInterrupcion();
}

void ejecutar_MOV_OUT(char* Registro_Datos, char* Registro_Direccion){
    void* reg_Direc = obtener_registro(Registro_Direccion);
    void* reg_Datos = obtener_registro(Registro_Datos);
    uint32_t direcLogi = *(uint32_t*)reg_Direc;
    int direccionLogica = (int)direcLogi;
    uint32_t valorr = *(uint32_t*)reg_Datos;
    int valor = (int)valorr;
    t_mmu_cpu* mmu_mov_in = traducirDireccion(direccionLogica);
    int tamanio_registro = espacio_de_registro(Registro_Datos);

    // t_paquete* solicitud_escritura = crear_paquete(ACCESO_A_ESCRITURA);
    // while (tamanio_registro > config_cpu->TAMANIO_MARCO){
    //     agregar_a_paquete(solicitud_escritura,&valor, config_cpu->TAMANIO_MARCO);
    //     tamanio_registro --;
    // }
    // if (tamanio_registro != 0){
    //     agregar_a_paquete(solicitud_escritura,&valor, tamanio_registro);
    // }
    
    send_escribi_memoria(pcb->pid,mmu_mov_in->direccionFIsica, tamanio_registro, valor); //Memoria me dijo que no hace falta el PID 
    //Noc como hacer la verificacion ver despues 
    if(recv_escribir_memoria() == 1){
        log_info(logger,"Se puedo escribir correctamente");
    }else{
        log_error(logger,"No se pudo escribir en memoria");
    }
    free(mmu_mov_in);
    tengoAlgunaInterrupcion();
}

void ejecutar_RESIZE (char* tam){
    int tamanio  = atoi(tam);
    
    send_agrandar_memoria(pcb->pid,tamanio);
    if(recv_agrandar_memoria()!= -1){
        log_info(logger,"Se pudo agrandar correctamente");
        tengoAlgunaInterrupcion();
    }else{
        log_info(logger,"NO!! se pudo agrandar correctamente");
        t_paquete* paquete_a_kernel = crear_paquete(OUT_OF_MEMORY);
        enviar_pcb_a_kernel(paquete_a_kernel);
    }
}


void ejecutar_COPY_STRING(char* tam){
    int tamanio = atoi(tam);
    void* registroSI = obtener_registro("SI");
    void* registroDI = obtener_registro ("DI");
    uint32_t regSI = *(uint32_t*) registroSI;
    int registerSI = (int)regSI;
    uint32_t regDI = *(uint32_t*) registroDI;
    t_mmu_cpu* mmu_copiar_string_SI = traducirDireccion(registerSI);

    enviar_a_leer_memoria(pcb->pid,mmu_copiar_string_SI->direccionFIsica,tamanio);
    char* valor = recv_escribir_memoria_string(tamanio);
    free(mmu_copiar_string_SI); 

    int registreDI = (int)regDI;
    t_mmu_cpu* mmu_copiar_string_DI = traducirDireccion(registreDI);

    send_escribi_memoria_string(pcb->pid,mmu_copiar_string_DI->direccionFIsica,tamanio, valor);
        if(recv_escribir_memoria() == 1){
        log_info(logger,"Se puedo escribir correctamente");
    }else{
        log_error(logger,"No se pudo escribir en memoria");
    }
    free(mmu_copiar_string_DI);
    tengoAlgunaInterrupcion();
}

void ejecutar_WAIT(char* recurso){
    t_paquete* paquete_a_kernel = crear_paquete(WAIT);
    agregar_a_paquete(paquete_a_kernel, &recurso, sizeof(char*));
    enviar_pcb_a_kernel(paquete_a_kernel);
}

void ejecutar_SINGAL(char* recurso){
    t_paquete* paquete_a_kernel = crear_paquete(SIGNAL);
    agregar_a_paquete(paquete_a_kernel, &recurso, sizeof(char*));
    enviar_pcb_a_kernel(paquete_a_kernel);
}






