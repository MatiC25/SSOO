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
           //og_error(logger, "Operacion desconocida");
       }
    }   
}

void ejecutar_ciclo_instrucciones(int socket_cliente, int socket_server) {
free(pcb);

    pcb = rcv_contexto_ejecucion_cpu(socket_cliente);
    printf("%i", pcb->pid);
    printf("%i", pcb->program_counter);

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
t_instruccion* instruccion = malloc(sizeof(t_instruccion));
     instruccion = recv_instruccion(config_cpu->SOCKET_MEMORIA);
      t_tipo_instruccion tipo_instruccion = obtener_tipo_instruccion(instruccion->opcode); //decode
            switch (tipo_instruccion)
        {
        case EXIT:
            //t_paquete* paquete_a_kernel = crear_paquete(EXIT);
            //enviar_pcb_a_kernel(paquete_a_kernel);

            return; 
            break;
        case SET:
            //ejecutar_set(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SUM:
            //ejecutar_sum(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SUB:
            //ejecutar_sub(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;           
        case JNZ:
            //ejecutar_JNZ(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_GEN_SLEEP:
           //ejecutar_IO_GEN_SLEEP(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case MOVE_IN:
           // ejecutar_MOV_IN(instruccion->parametro1, instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case MOV_OUT:
           //ejecutar_MOV_OUT(instruccion->parametro1, instruccion->parametro2);
           log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case RESIZE:
//ejecutar_RESIZE(instruccion->parametro1);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s ", pcb->pid,instruccion->opcode,instruccion->parametro1);
            break;
        case COPY_STRING:
           // ejecutar_COPY_STRING(instruccion->parametro1);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s ", pcb->pid,instruccion->opcode,instruccion->parametro1);
            break;
        case WAIT:
           // ejecutar_WAIT(instruccion->parametro1);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s", pcb->pid,instruccion->opcode,instruccion->parametro1);
            break;
        case SIGNAL:
           // ejecutar_SINGAL(instruccion->parametro1);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s", pcb->pid,instruccion->opcode,instruccion->parametro1);
            break;
        case IO_STDIN_READ:
           // ejecutar_IO_STDIN_READ(instruccion->parametro1, instruccion->parametro2,instruccion->parametro3);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2, instruccion->parametro3);
        case IO_STDOUT_WRITE:
            //ejecutar_IO_STDOUT_WRITE(instruccion->parametro1, instruccion->parametro2,instruccion->parametro3);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2, instruccion->parametro3);
            break;
        case IO_FS_CREATE:
            //ejecutar_IO_FS_CREATE(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_DELETE:
            //ejecutar_IO_FS_DELETE(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_TRUNCATE:
            //ejecutar_IO_FS_TRUNCATE(instruccion->parametro1, instruccion->parametro2,instruccion->parametro3);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2, instruccion->parametro3);
            break;
        case IO_FS_WRITE:
            //ejecutar_IO_FD_WRITE(instruccion->parametro1,instruccion->parametro2,instruccion->parametro3,instruccion->parametro4,instruccion->parametro5);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s %s %s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2,instruccion->parametro3,instruccion->parametro4,instruccion->parametro5);
            break;       
         case IO_FS_READ:
            //ejecutar_IO_FS_READ(instruccion->parametro1,instruccion->parametro2,instruccion->parametro3,instruccion->parametro4,instruccion->parametro5);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s %s %s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2,instruccion->parametro3,instruccion->parametro4,instruccion->parametro5);
            break;
        default:
        log_error(logger, "Operacion desconocida");          
        }
}

// void ejecutar_set (char* registro, char* valor){
//     void* reg = obtener_registro(registro);
//     if(reg != NULL){
//         operar_con_registros(reg,NULL,registro,"set",atoi(valor));
//     }else{
//         log_error(logger,"Error al obtener el SET");
//     }
//     tengoAlgunaInterrupcion();

// }

// void ejecutar_sum (char* registro_origen_char, char* registro_desitino_char) {
//     void* registro_origen = obtener_registro(registro_origen_char);
//     void* registro_destino = obtener_registro(registro_desitino_char);

//     if (registro_origen != NULL && registro_destino != NULL)
//     {
//         operar_con_registros(registro_destino,registro_origen,registro_origen_char, "+" ,0);
//     }else{
//         log_error(logger,"Error al obtener el SUM");
//     }
//     tengoAlgunaInterrupcion();  
// }


// void ejecutar_sub (char* registro_origen_char, char* registro_desitino_char){
//     void* registro_origen = obtener_registro(registro_origen_char);
//     void* registro_destino = obtener_registro(registro_desitino_char);

//        if (registro_origen != NULL && registro_destino != NULL)
//     {
//         operar_con_registros(registro_destino,registro_origen,registro_origen_char, "-" , 0);
//     }else{
//         log_error(logger,"Error al obtener el SUB");
//     }
//     tengoAlgunaInterrupcion();
// }

// void ejecutar_JNZ(char* registro, char* valor){
//     void* reg = obtener_registro(registro);
//     if (reg == 0)
//     {
//         pcb->program_counter += atoi(valor);
//     }else{
//         log_error(logger,"Error al obtener el JNZ");
//     }
//     tengoAlgunaInterrupcion();
// }

// void ejecutar_IO_GEN_SLEEP(char* interfazAUsar, char* tiempoDeTrabajo){
//     t_paquete* paquete_a_kernel = crear_paquete(OPERACION_IO);
//     agregar_a_paquete(paquete_a_kernel, &interfazAUsar, strlen(interfazAUsar) * sizeof(char));
//     agregar_a_paquete(paquete_a_kernel, &tiempoDeTrabajo, strlen(tiempoDeTrabajo) * sizeof(char));

//     enviar_pcb_a_kernel(paquete_a_kernel);
//     // Enviar_Dato_Por_Dispacht
// }


// void ejecutar_MOV_IN(char* registro_Datos, char* registro_Direccion){
//     void* reg_Direccion = obtener_registro(registro_Direccion);
//     void* reg_Datos   = obtener_registro(registro_Datos);
//     uint32_t direcLogi = *(uint32_t*)reg_Direccion;
//     int direccionLogica = (int)direcLogi;
//     int tamanio_registro = espacio_de_registro(registro_Datos);
//     t_mmu_cpu* mmu_mov_in = traducirDireccion(direccionLogica, tamanio_registro);
    
// char* valor = comunicaciones_con_memoria_lectura(mmu_mov_in);

//     operar_con_registros(reg_Datos,NULL,registro_Datos,"set",atoi(valor));
//     free(valor);
//     free(mmu_mov_in);
//     void tengoAlgunaInterrupcion();
// }

// void ejecutar_MOV_OUT(char* Registro_Datos, char* Registro_Direccion){
//     void* reg_Direc = obtener_registro(Registro_Direccion);
//     void* reg_Datos = obtener_registro(Registro_Datos);
//     uint32_t direcLogi = *(uint32_t*)reg_Direc;
//     int direccionLogica = (int)direcLogi;
//     char* valorr = (char*)reg_Datos;
    
//     int tamanio_registro = espacio_de_registro(Registro_Datos);
//     t_mmu_cpu* mmu_mov_out = traducirDireccion(direccionLogica, tamanio_registro);
    
//     //Noc como hacer la verificacion ver despues 
//     if(comunicaciones_con_memoria_escritura(mmu_mov_out, valorr) == 1){
//         log_info(logger,"Se puedo escribir correctamente");
//     }else{
//         log_error(logger,"No se pudo escribir en memoria");
//     }
//     free(mmu_mov_out);
//     tengoAlgunaInterrupcion();
// }

// void ejecutar_RESIZE (char* tam){
//     int tamanio  = atoi(tam);
    
//     send_agrandar_memoria(pcb->pid,tamanio);
//     if(recv_agrandar_memoria()!= -1){
//         log_info(logger,"Se pudo agrandar correctamente");
//         tengoAlgunaInterrupcion();
//     }else{
//         log_info(logger,"NO!! se pudo agrandar correctamente");
//         t_paquete* paquete_a_kernel = crear_paquete(OUT_OF_MEMORY);
//         enviar_pcb_a_kernel(paquete_a_kernel);
//         free(paquete_a_kernel);
//     }
// }


// void ejecutar_COPY_STRING(char* tam){
//     int tamanio = atoi(tam);
//     void* registroSI = obtener_registro("SI");
//     void* registroDI = obtener_registro ("DI");
//     uint32_t regSI = *(uint32_t*) registroSI;
//     int registerSI = (int)regSI;
//     uint32_t regDI = *(uint32_t*) registroDI;
//     t_mmu_cpu* mmu_copiar_string_SI = traducirDireccion(registerSI, tamanio);

//     char* valor = comunicaciones_con_memoria_lectura(mmu_copiar_string_SI);
//     free(mmu_copiar_string_SI); 

//     int registreDI = (int)regDI;
//     t_mmu_cpu* mmu_copiar_string_DI = traducirDireccion(registreDI, tamanio);

//         if(comunicaciones_con_memoria_escritura(mmu_copiar_string_DI, valor) == 1){
//         log_info(logger,"Se puedo escribir correctamente");
//     }else{
//         log_error(logger,"No se pudo escribir en memoria");
//     }
//     free(valor);
//     free(mmu_copiar_string_DI);
//     tengoAlgunaInterrupcion();
// }

// void ejecutar_WAIT(char* recurso){
//     t_paquete* paquete_a_kernel = crear_paquete(WAIT);
//     enviar_pcb_a_kernel(paquete_a_kernel);
//     agregar_a_paquete(paquete_a_kernel, &recurso, strlen(recurso) * sizeof(char));
//     enviar_paquete(paquete_a_kernel, config_cpu->SOCKET_KERNEL);
//     eliminar_paquete(paquete_a_kernel);
// }

// void ejecutar_SINGAL(char* recurso){
//     t_paquete* paquete_a_kernel = crear_paquete(SIGNAL);
//     enviar_pcb_a_kernel(paquete_a_kernel);
//     agregar_a_paquete(paquete_a_kernel, &recurso, strlen(recurso) * sizeof(char));
//     enviar_paquete(paquete_a_kernel, config_cpu->SOCKET_KERNEL);
//     eliminar_paquete(paquete_a_kernel);
// }


// void ejecutar_IO_STDIN_READ(char* interfaz, char* registro_direccion, char* registro_tamanio){
//     void* registroDireccion = obtener_registro(registro_direccion);
//     void* registroTamanio  = obtener_registro(registro_tamanio);
//     uint32_t regDireccion = *(uint32_t*) registroDireccion;
//     int reg_Direc = (int)regDireccion;
//     uint32_t regTamanio   = *(uint32_t*) registroTamanio; 
//     int reg_Tamanio = (int) regTamanio;
//     t_mmu_cpu * mmu_io_stdin_read = traducirDireccion(reg_Direc,reg_Tamanio);
//     t_paquete* paquete_std = crear_paquete(OPERACION_IO);
//     solicitar_a_kernel_std(interfaz,reg_Tamanio, mmu_io_stdin_read->direccionFIsica,paquete_std);  
//     enviar_paquete(paquete_std, config_cpu->SOCKET_KERNEL);
//     eliminar_paquete(paquete_std);
//     free(mmu_io_stdin_read);
// }

// void ejecutar_IO_STDOUT_WRITE(char* interfaz, char* registro_direccion, char* registro_tamanio){
//     void* registroDireccion = obtener_registro(registro_direccion);
//     void* registroTamanio  = obtener_registro(registro_tamanio);
//     uint32_t regDireccion = *(uint32_t*) registroDireccion;
//     int reg_Direc = (int)regDireccion;
//     uint32_t regTamanio   = *(uint32_t*) registroTamanio; 
//     int reg_Tamanio = (int) regTamanio;
//     t_mmu_cpu * mmu_io_stdout_write = traducirDireccion(reg_Direc,reg_Tamanio);
//     t_paquete* paquete_std = crear_paquete(OPERACION_IO);
//     solicitar_a_kernel_std(interfaz, reg_Tamanio,mmu_io_stdout_write->direccionFIsica,paquete_std);
//     enviar_paquete(paquete_std, config_cpu->SOCKET_KERNEL);
//     eliminar_paquete(paquete_std);
//     free(mmu_io_stdout_write);
    
// }


// void ejecutar_IO_FS_CREATE(char* interfaz, char* nombre_archibo){
//     t_paquete* paquete_IO = crear_paquete(OPERACION_IO);
//     enviar_pcb_a_kernel(paquete_IO);
//     agregar_a_paquete(paquete_IO, &interfaz, strlen(interfaz) * sizeof(char));
//     agregar_a_paquete(paquete_IO, &nombre_archibo, strlen(nombre_archibo) * sizeof(char));
//     enviar_paquete(paquete_IO, config_cpu->SOCKET_KERNEL);
//     eliminar_paquete(paquete_IO); 
// }

// void ejecutar_IO_FS_DELETE(char* interfaz, char* nombre_archibo){
//     t_paquete* paquete_IO = crear_paquete(OPERACION_IO);
//     enviar_pcb_a_kernel(paquete_IO);
//     agregar_a_paquete(paquete_IO, &interfaz, strlen(interfaz) * sizeof(char));
//     agregar_a_paquete(paquete_IO, &nombre_archibo, strlen(nombre_archibo) * sizeof(char));
//     enviar_paquete(paquete_IO, config_cpu->SOCKET_KERNEL);
//     eliminar_paquete(paquete_IO); 
// }

// void ejecutar_IO_FS_TRUNCATE(char* interfaz, char* nombre_archivo, char* registro_tamanio){
//     void* registroTamanio  = obtener_registro(registro_tamanio);
//     uint32_t regTamanio   = *(uint32_t*) registroTamanio; 
//     int reg_Tamanio = (int) regTamanio;
//     t_paquete* paquete_IO = crear_paquete(OPERACION_IO);
//     enviar_pcb_a_kernel(paquete_IO);
//     agregar_a_paquete(paquete_IO, &interfaz, strlen(interfaz) * sizeof(char));
//     agregar_a_paquete(paquete_IO, &nombre_archivo, strlen(nombre_archivo) * sizeof(char));
//     agregar_a_paquete(paquete_IO, &reg_Tamanio, sizeof(int));
//     enviar_paquete(paquete_IO, config_cpu->SOCKET_KERNEL);
//     eliminar_paquete(paquete_IO);
// }

// void ejecutar_IO_FD_WRITE(char* interfaz, char* nombre_archivo, char* registro_direccion, char* registro_tamanio, char* registro_puntero_archivo){
//     void* registroDireccion = obtener_registro(registro_direccion);
//     uint32_t regDireccion = *(uint32_t*) registroDireccion;
//     int reg_Direc = (int)regDireccion;
//     void* registroTamanio  = obtener_registro(registro_tamanio);
//     uint32_t regTamanio   = *(uint32_t*) registroTamanio; 
//     int reg_Tamanio = (int) regTamanio;
//     void* registroArchivo  = obtener_registro(registro_puntero_archivo);
//     uint32_t regArchivo = *(uint32_t*) registroArchivo;
//     int reg_Archi = (int)regArchivo;
//     t_mmu_cpu * mmu_io_fs_write = traducirDireccion(reg_Direc,reg_Tamanio);
//     char* valor = comunicaciones_con_memoria_lectura(mmu_io_fs_write);
//     t_paquete* paquete_IO = crear_paquete(OPERACION_IO);
//     enviar_pcb_a_kernel(paquete_IO);
//     agregar_a_paquete(paquete_IO, &interfaz, strlen(interfaz) * sizeof(char));
//     agregar_a_paquete(paquete_IO, &nombre_archivo, strlen(nombre_archivo) * sizeof(char));
//     agregar_a_paquete(paquete_IO, &valor, strlen(valor) * sizeof(char));
//     agregar_a_paquete(paquete_IO, &reg_Tamanio, sizeof(int)); //Noc si hace falta
//     agregar_a_paquete(paquete_IO, &reg_Archi, sizeof(int)); //Porsicion en archivo 
//     enviar_paquete(paquete_IO, config_cpu->SOCKET_KERNEL);
//     eliminar_paquete(paquete_IO);
//     free(mmu_io_fs_write);
// }


// void ejecutar_IO_FS_READ(char* interfaz, char* nombre_archivo, char* registro_direccion, char* registro_tamanio, char* registro_puntero_archivo){
//     void* registroDireccion = obtener_registro(registro_direccion);
//     uint32_t regDireccion = *(uint32_t*) registroDireccion;
//     int reg_Direc = (int)regDireccion;
//     void* registroTamanio  = obtener_registro(registro_tamanio);
//     uint32_t regTamanio   = *(uint32_t*) registroTamanio; 
//     int reg_Tamanio = (int) regTamanio;
//     void* registroArchivo  = obtener_registro(registro_puntero_archivo);
//     uint32_t regArchivo = *(uint32_t*) registroArchivo;
//     int reg_Archi = (int)regArchivo;
//     t_mmu_cpu * mmu_io_fs_read = traducirDireccion(reg_Direc,reg_Tamanio);
//     t_paquete* paquete_IO = crear_paquete(OPERACION_IO);
//     enviar_pcb_a_kernel(paquete_IO);
//     agregar_a_paquete(paquete_IO, &interfaz, strlen(interfaz) * sizeof(char));
//     agregar_a_paquete(paquete_IO, &nombre_archivo, strlen(nombre_archivo) * sizeof(char));
//     agregar_a_paquete(paquete_IO, &reg_Tamanio, sizeof(int));
//     agregar_a_paquete(paquete_IO, &reg_Archi, sizeof(int)); //Porsicion en archivo 
//        while (list_is_empty(mmu_io_fs_read->direccionFIsica)){
//         int* direccion_fisica = (int*)list_remove(mmu_io_fs_read->direccionFIsica, 0);
//         agregar_a_paquete(paquete_IO, direccion_fisica, sizeof(int));
//         free(direccion_fisica);
//     }
//     enviar_paquete(paquete_IO, config_cpu->SOCKET_KERNEL);
//     eliminar_paquete(paquete_IO);
//     free(mmu_io_fs_read);

// }