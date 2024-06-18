#include "cpu-ciclo-instrucciones.h"
//BRANCH DE FEDE juntado con branch leo nico
t_pcb_cpu* pcb;
int seguir_ejecutando;

void iniciar_ciclo_de_ejecucion(int socket_server ,int socket_cliente) {
    //log_info(logger,"socket DS %i", socket_cliente);
    config_cpu->SOCKET_KERNEL = socket_cliente;
    while(1) {
        int codigo_operacion = recibir_operacion(socket_cliente); //Acordarme que lo cambie
        switch(codigo_operacion) {  
            case RECIBIR_PROCESO:
            log_warning(logger,"Recibiendo la PCB");
            ejecutar_ciclo_instrucciones(socket_cliente, socket_server);
            break;
            case HANDSHAKE:
            //log_warning(logger,"Haciendo HANDSHAKE");
            recibir_handshake(socket_cliente);
            break;
            case -1:
            log_warning(logger,"Se desconecto el cliente Kernel (Ds)");
            return ; 
        default:
        log_error(logger, "Operacion desconocida");
       }
    }   
}

void ejecutar_ciclo_instrucciones(int socket_cliente, int socket_server) {
    
    pcb = rcv_contexto_ejecucion_cpu(socket_cliente);
    seguir_ciclo(socket_cliente, socket_server);

}   

void seguir_ciclo(){
    fecth(config_cpu->SOCKET_MEMORIA);
    ejecutar_instruccion(config_cpu->SOCKET_MEMORIA);

}

void fecth(int socket_server){
    int program_counter = 0;
    int PID = pcb->pid;
    program_counter = pcb->program_counter++;
    solicitar_instruccion(socket_server,PID, program_counter);
    log_info(logger,"Fetch Instruccion: PID: %d - FETCH -Programn Counter: %d",PID,program_counter);   
}

void* obtener_registro (char *registro) {
    if(strncmp(registro, "AX", 2) == 0) {
        return &(pcb->registros->AX);
    } else if(strncmp(registro, "BX", 2) == 0) {
        return &(pcb->registros->BX);
    } else if(strncmp(registro, "CX", 2) == 0) {
        return &(pcb->registros->CX);
    } else if(strncmp(registro, "DX", 2) == 0) {
        return &(pcb->registros->DX);
    } else if(strncmp(registro, "PC", 2) == 0) {
        return &(pcb->registros->PC);
    } else if(strncmp(registro, "EAX", 3) == 0) {
        return &(pcb->registros->EAX);
    } else if(strncmp(registro, "EBX", 3) == 0) {
        return &(pcb->registros->EBX);
    }else if(strncmp(registro, "ECX", 3) == 0) {
        return &(pcb->registros->ECX);
    }else if(strncmp(registro, "EDX", 3) == 0) {
        return &(pcb->registros->EDX);
    }else if(strncmp(registro, "SI", 2) == 0) {
        return &(pcb->registros->SI);
    }else if(strncmp(registro, "DI", 2) == 0) {
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
        if (strcmp(operacion, "+") == 0){
            *(uint8_t*)registro_destino += *(uint8_t*) registro_origen;
        }else if (strcmp(operacion, "-") == 0){
            *(uint8_t*)registro_destino = *(uint8_t*)registro_destino - *(uint8_t*) registro_origen;
        }else if(strcmp(operacion, "set") == 0){
            *(uint8_t*)registro_destino = (uint8_t)valor;
        }else{  
            log_error(logger, "los registros no son de 8 bits");
        }
    }else if(strcmp(registro, "PC") == 0 || strcmp(registro, "EAX") == 0|| strcmp(registro, "EBX") == 0 || strcmp(registro, "ECX") == 0 || strcmp(registro, "EDX") == 0 || strcmp(registro, "SI") == 0|| strcmp(registro, "DI") == 0){
        if (strcmp(operacion, "+") == 0){
            *(uint32_t*)registro_destino += *(uint32_t*) registro_origen;
        }else if (strcmp(operacion, "-") == 0){
            *(uint32_t*)registro_destino = *(uint32_t*)registro_destino - *(uint32_t*) registro_origen;
        }else if(strcmp(operacion, "set") == 0){
            *(uint32_t*)registro_destino = (uint32_t)valor;
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
    return;
    }else{
        seguir_ciclo();
    }  
}

void ejecutar_instruccion(int socket_cliente) {
    t_instruccion *instruccion = recv_instruccion(socket_cliente);
    t_tipo_instruccion tipo_instruccion = obtener_tipo_instruccion(instruccion->opcode); //decode

        switch (tipo_instruccion){
        case EXIT:
            mostrar_pcb(pcb);
            log_info(logger,"Instruccion Ejecutada: PID: %i Ejecutando: %s", pcb->pid,instruccion->opcode);
            t_paquete* paquete_a_kernel = crear_paquete(EXIT);
            enviar_pcb_a_kernel(paquete_a_kernel);
            enviar_paquete(paquete_a_kernel, config_cpu->SOCKET_KERNEL);
            eliminar_paquete(paquete_a_kernel);
            return; 
        case SET:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            ejecutar_set(instruccion->parametro1,instruccion->parametro2);
            break;
        case SUM:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            ejecutar_sum(instruccion->parametro1,instruccion->parametro2);
            break;
        case SUB:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            ejecutar_sub(instruccion->parametro1,instruccion->parametro2);
            break;           
        case JNZ:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            ejecutar_JNZ(instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_GEN_SLEEP:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            ejecutar_IO_GEN_SLEEP(instruccion->parametro1,instruccion->parametro2);
            break;
        case MOVE_IN:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            ejecutar_MOV_IN(instruccion->parametro1, instruccion->parametro2);
            break;
        case MOV_OUT:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            ejecutar_MOV_OUT(instruccion->parametro1, instruccion->parametro2);
            break;
        case RESIZE:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s ", pcb->pid,instruccion->opcode,instruccion->parametro1);
            ejecutar_RESIZE(instruccion->parametro1);
            break;
        case COPY_STRING:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s ", pcb->pid,instruccion->opcode,instruccion->parametro1);
            ejecutar_COPY_STRING(instruccion->parametro1);
            break;
        case WAIT:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s", pcb->pid,instruccion->opcode,instruccion->parametro1);
            ejecutar_WAIT(instruccion->parametro1);
            break;
        case SIGNAL:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s", pcb->pid,instruccion->opcode,instruccion->parametro1);
            ejecutar_SINGAL(instruccion->parametro1);
            break;
        case IO_STDIN_READ:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2, instruccion->parametro3);
            ejecutar_IO_STDIN_READ(instruccion->parametro1, instruccion->parametro2,instruccion->parametro3);
        case IO_STDOUT_WRITE:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2, instruccion->parametro3);
            ejecutar_IO_STDOUT_WRITE(instruccion->parametro1, instruccion->parametro2,instruccion->parametro3);
            break;
        case IO_FS_CREATE:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            ejecutar_IO_FS_CREATE(instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_DELETE:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            ejecutar_IO_FS_DELETE(instruccion->parametro1,instruccion->parametro2);
            break;
        case IO_FS_TRUNCATE:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2, instruccion->parametro3);
            ejecutar_IO_FS_TRUNCATE(instruccion->parametro1, instruccion->parametro2,instruccion->parametro3);
            break;
        case IO_FD_WRITE:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s %s %s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2,instruccion->parametro3,instruccion->parametro4,instruccion->parametro5);
            ejecutar_IO_FD_WRITE(instruccion->parametro1,instruccion->parametro2,instruccion->parametro3,instruccion->parametro4,instruccion->parametro5);
            break;       
         case IO_FS_READ:
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s %s %s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2,instruccion->parametro3,instruccion->parametro4,instruccion->parametro5);
            ejecutar_IO_FS_READ(instruccion->parametro1,instruccion->parametro2,instruccion->parametro3,instruccion->parametro4,instruccion->parametro5);
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


void ejecutar_sub (char* registro_desitino_char, char* registro_origen_char){
    void* registro_origen = obtener_registro(registro_origen_char);
    void* registro_destino = obtener_registro(registro_desitino_char);

       if (registro_origen != NULL && registro_destino != NULL)
    {
        operar_con_registros(registro_destino,registro_origen,registro_desitino_char, "-" , 0);
    }else{
        log_error(logger,"Error al obtener el SUB");
    }
    tengoAlgunaInterrupcion();
}

void ejecutar_JNZ(char* registro, char* valor){
    void* reg = obtener_registro(registro);
    uint32_t regg = *(uint32_t*)reg;
    if (regg == 0){
        pcb->program_counter += atoi(valor);
    }else{
        log_error(logger,"Error al obtener el JNZ");
    }
    tengoAlgunaInterrupcion();
}

void ejecutar_IO_GEN_SLEEP(char* interfazAUsar, char* tiempoDeTrabajo){
    mostrar_pcb(pcb);
    int tiempo = atoi(tiempoDeTrabajo);
    t_paquete* paquete_a_kernel = crear_paquete(OPERACION_IO);
    enviar_pcb_a_kernel(paquete_a_kernel);
    enviar_paquete(paquete_a_kernel,config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_a_kernel);

    int respuesta;

    recv(config_cpu->SOCKET_KERNEL,&respuesta , sizeof(int), MSG_WAITALL);
    log_warning(logger, "Numero de respuesta es %i", respuesta);

    if(respuesta == 1){
        t_paquete* paquete = crear_paquete(IO_GEN_SLEEP);
        agregar_a_paquete_string(paquete_a_kernel, interfazAUsar, strlen(interfazAUsar) + 1);
        agregar_a_paquete (paquete_a_kernel ,&tiempo, sizeof(int));
        enviar_paquete(paquete_a_kernel,config_cpu->SOCKET_KERNEL);
        eliminar_paquete(paquete_a_kernel);
    }else{log_error(logger , "Erro en la respuesta de desalojo de I/O");}
}


void ejecutar_MOV_IN(char* registro_Datos, char* registro_Direccion){
    void* reg_Direccion = obtener_registro(registro_Direccion);
    void* reg_Datos   = obtener_registro(registro_Datos);
    uint32_t direcLogi = *(uint32_t*)reg_Direccion;
    int direccionLogica = (int)direcLogi;
    int tamanio_registro = espacio_de_registro(registro_Datos);
    t_mmu_cpu* mmu_mov_in = traducirDireccion(direccionLogica, tamanio_registro);
    
char* valor = comunicaciones_con_memoria_lectura(mmu_mov_in);

    operar_con_registros(reg_Datos,NULL,registro_Datos,"set",atoi(valor));
    free(valor);
    free(mmu_mov_in);
    void tengoAlgunaInterrupcion();
}

void ejecutar_MOV_OUT(char* Registro_Datos, char* Registro_Direccion){
    void* reg_Direc = obtener_registro(Registro_Direccion);
    void* reg_Datos = obtener_registro(Registro_Datos);
    uint32_t direcLogi = *(uint32_t*)reg_Direc;
    int direccionLogica = (int)direcLogi;
    char* valorr = (char*)reg_Datos;
    
    int tamanio_registro = espacio_de_registro(Registro_Datos);
    t_mmu_cpu* mmu_mov_out = traducirDireccion(direccionLogica, tamanio_registro);
    
    //Noc como hacer la verificacion ver despues 
    if(comunicaciones_con_memoria_escritura(mmu_mov_out, valorr) == 1){
        log_info(logger,"Se puedo escribir correctamente");
    }else{
        log_error(logger,"No se pudo escribir en memoria");
    }
    free(mmu_mov_out);
    tengoAlgunaInterrupcion();
}

void ejecutar_RESIZE(char* tam){
    int tamanio  = atoi(tam);
    
    send_agrandar_memoria(pcb->pid,tamanio);
    if(recv_agrandar_memoria()!= -1){
        log_info(logger,"Se pudo agrandar correctamente");
        tengoAlgunaInterrupcion();
    }else{
        log_info(logger,"NO!! se pudo agrandar correctamente");
        t_paquete* paquete_a_kernel = crear_paquete(OUT_OF_MEMORY);
        enviar_pcb_a_kernel(paquete_a_kernel);
        free(paquete_a_kernel);
    }
}


void ejecutar_COPY_STRING(char* tam){
    int tamanio = atoi(tam);
    void* registroSI = obtener_registro("SI");
    void* registroDI = obtener_registro ("DI");
    uint32_t regSI = *(uint32_t*) registroSI;
    int registerSI = (int)regSI;
    uint32_t regDI = *(uint32_t*) registroDI;
    t_mmu_cpu* mmu_copiar_string_SI = traducirDireccion(registerSI, tamanio);

    char* valor = comunicaciones_con_memoria_lectura(mmu_copiar_string_SI);
    free(mmu_copiar_string_SI); 

    int registreDI = (int)regDI;
    t_mmu_cpu* mmu_copiar_string_DI = traducirDireccion(registreDI, tamanio);

        if(comunicaciones_con_memoria_escritura(mmu_copiar_string_DI, valor) == 1){
        log_info(logger,"Se puedo escribir correctamente");
    }else{
        log_error(logger,"No se pudo escribir en memoria");
    }
    free(valor);
    free(mmu_copiar_string_DI);
    tengoAlgunaInterrupcion();
}

void ejecutar_WAIT(char* recurso){
    t_paquete* paquete_a_kernel = crear_paquete(WAIT);
    enviar_pcb_a_kernel(paquete_a_kernel);
    agregar_a_paquete_string(paquete_a_kernel, recurso, strlen(recurso) + 1);
    enviar_paquete(paquete_a_kernel, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_a_kernel);
}

void ejecutar_SINGAL(char* recurso){
    t_paquete* paquete_a_kernel = crear_paquete(SIGNAL);
    enviar_pcb_a_kernel(paquete_a_kernel);
    agregar_a_paquete_string(paquete_a_kernel, recurso, strlen(recurso) + 1);
    enviar_paquete(paquete_a_kernel, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_a_kernel);
}


void ejecutar_IO_STDIN_READ(char* interfaz, char* registro_direccion, char* registro_tamanio){
    void* registroDireccion = obtener_registro(registro_direccion);
    void* registroTamanio  = obtener_registro(registro_tamanio);
    uint32_t regDireccion = *(uint32_t*) registroDireccion;
    int reg_Direc = (int)regDireccion;
    uint32_t regTamanio   = *(uint32_t*) registroTamanio; 
    int reg_Tamanio = (int) regTamanio;
    t_mmu_cpu * mmu_io_stdin_read = traducirDireccion(reg_Direc,reg_Tamanio);
    t_paquete* paquete_std = crear_paquete(OPERACION_IO); // dejo asi 
    enviar_pcb_a_kernel(paquete_std );   
    enviar_paquete(paquete_std , config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_std );

    t_paquete* paquete_stdin = crear_paquete(STDIN);
    solicitar_a_kernel_std(interfaz,reg_Tamanio, mmu_io_stdin_read->direccionFIsica,paquete_stdin);  ;
    free(mmu_io_stdin_read);
}

void ejecutar_IO_STDOUT_WRITE(char* interfaz, char* registro_direccion, char* registro_tamanio){
    void* registroDireccion = obtener_registro(registro_direccion);
    void* registroTamanio  = obtener_registro(registro_tamanio);
    uint32_t regDireccion = *(uint32_t*) registroDireccion;
    int reg_Direc = (int)regDireccion;
    uint32_t regTamanio   = *(uint32_t*) registroTamanio; 
    int reg_Tamanio = (int) regTamanio;
    t_mmu_cpu * mmu_io_stdout_write = traducirDireccion(reg_Direc,reg_Tamanio);
    t_paquete* paquete_std = crear_paquete(OPERACION_IO); // dejo asi 
    enviar_pcb_a_kernel(paquete_std);   
    enviar_paquete(paquete_std, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_std);


    t_paquete* paquete_stdout = crear_paquete(STDOUT); // dejo asi 
    solicitar_a_kernel_std(interfaz, reg_Tamanio,mmu_io_stdout_write->direccionFIsica,paquete_stdout);
    enviar_paquete(paquete_stdout, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_stdout);
    free(mmu_io_stdout_write);
    
}


void ejecutar_IO_FS_CREATE(char* interfaz, char* nombre_archibo){
    t_paquete* paquete_IO = crear_paquete(OPERACION_IO);
    enviar_pcb_a_kernel(paquete_IO);
    enviar_paquete(paquete_IO, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_IO);

    t_paquete* paquet_fs_create = crear_paquete(IO_FS_CREATE);
    agregar_a_paquete_string(paquet_fs_create, interfaz, strlen(interfaz) + 1);
    agregar_a_paquete_string(paquet_fs_create, nombre_archibo, strlen(nombre_archibo) + 1);
    enviar_paquete(paquet_fs_create, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquet_fs_create); 
}

void ejecutar_IO_FS_DELETE(char* interfaz, char* nombre_archibo){
    t_paquete* paquete_IO = crear_paquete(OPERACION_IO);
    enviar_pcb_a_kernel(paquete_IO);
    enviar_paquete(paquete_IO, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_IO);


    t_paquete* paquete_delete = crear_paquete(IO_FS_DELETE);
    agregar_a_paquete_string(paquete_delete, interfaz, strlen(interfaz ) + 1);
    agregar_a_paquete_string(paquete_delete, nombre_archibo, strlen(nombre_archibo ) + 1);  
    enviar_paquete(paquete_delete, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_delete); 
}

void ejecutar_IO_FS_TRUNCATE(char* interfaz, char* nombre_archivo, char* registro_tamanio){
    void* registroTamanio  = obtener_registro(registro_tamanio);
    uint32_t regTamanio   = *(uint32_t*) registroTamanio; 
    int reg_Tamanio = (int) regTamanio;
    t_paquete* paquete_IO = crear_paquete(OPERACION_IO);
    enviar_pcb_a_kernel(paquete_IO);
    enviar_paquete(paquete_IO, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_IO);


    t_paquete* paquete = crear_paquete(IO_FS_TRUNCATE);
    agregar_a_paquete_string(paquete, interfaz, strlen(interfaz) + 1);
    agregar_a_paquete_string(paquete, nombre_archivo, strlen(nombre_archivo) + 1);
    agregar_a_paquete(paquete, &reg_Tamanio, sizeof(int));
    enviar_paquete(paquete, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete);
}

void ejecutar_IO_FD_WRITE(char* interfaz, char* nombre_archivo, char* registro_direccion, char* registro_tamanio, char* registro_puntero_archivo){
    void* registroDireccion = obtener_registro(registro_direccion);
    uint32_t regDireccion = *(uint32_t*) registroDireccion;
    int reg_Direc = (int)regDireccion;
    void* registroTamanio  = obtener_registro(registro_tamanio);
    uint32_t regTamanio   = *(uint32_t*) registroTamanio; 
    int reg_Tamanio = (int) regTamanio;
    void* registroArchivo  = obtener_registro(registro_puntero_archivo);
    uint32_t regArchivo = *(uint32_t*) registroArchivo;
    int reg_Archi = (int)regArchivo;
    t_mmu_cpu * mmu_io_fs_write = traducirDireccion(reg_Direc,reg_Tamanio);
    char* valor = comunicaciones_con_memoria_lectura(mmu_io_fs_write);
    t_paquete* paquete_IO = crear_paquete(OPERACION_IO);
    enviar_pcb_a_kernel(paquete_IO);
    enviar_paquete(paquete_IO, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_IO);


    t_paquete* paqute = crear_paquete(IO_FD_WRITE);
    agregar_a_paquete_string(paqute, interfaz, strlen(interfaz) + 1);
    agregar_a_paquete_string(paqute, nombre_archivo, strlen(nombre_archivo) + 1);
    agregar_a_paquete_string(paqute, valor, strlen(valor) + 1);
    agregar_a_paquete(paqute, &reg_Tamanio, sizeof(int)); //Noc si hace falta
    agregar_a_paquete(paqute, &reg_Archi, sizeof(int)); //Porsicion en archivo 
    enviar_paquete(paqute, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paqute);
    free(mmu_io_fs_write);
}


void ejecutar_IO_FS_READ(char* interfaz, char* nombre_archivo, char* registro_direccion, char* registro_tamanio, char* registro_puntero_archivo){
    void* registroDireccion = obtener_registro(registro_direccion);
    uint32_t regDireccion = *(uint32_t*) registroDireccion;
    int reg_Direc = (int)regDireccion;
    void* registroTamanio  = obtener_registro(registro_tamanio);
    uint32_t regTamanio   = *(uint32_t*) registroTamanio; 
    int reg_Tamanio = (int) regTamanio;
    void* registroArchivo  = obtener_registro(registro_puntero_archivo);
    uint32_t regArchivo = *(uint32_t*) registroArchivo;
    int reg_Archi = (int)regArchivo;
    t_mmu_cpu * mmu_io_fs_read = traducirDireccion(reg_Direc,reg_Tamanio);
    t_paquete* paquete_IO = crear_paquete(OPERACION_IO);
    enviar_pcb_a_kernel(paquete_IO);
    enviar_paquete(paquete_IO, config_cpu->SOCKET_KERNEL);


    t_paquete* paquete = crear_paquete(IO_FS_READ);
    agregar_a_paquete_string(paquete, interfaz, strlen(interfaz) + 1);
    agregar_a_paquete_string(paquete, nombre_archivo, strlen(nombre_archivo) + 1);
    agregar_a_paquete(paquete, &reg_Tamanio, sizeof(int));
    agregar_a_paquete(paquete, &reg_Archi, sizeof(int)); //Porsicion en archivo 
       while (list_is_empty(mmu_io_fs_read->direccionFIsica)){
        int* direccion_fisica = (int*)list_remove(mmu_io_fs_read->direccionFIsica, 0);
        agregar_a_paquete(paquete, direccion_fisica, sizeof(int));
        free(direccion_fisica);
    }
    enviar_paquete(paquete, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete);
    free(mmu_io_fs_read);

}