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


void seguir_ciclo(int socket_cliente, int socket_server){
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
void operar_con_registros(void* registro_destino, void* registro_origen, char* registro, char* operacion, int valor){
    if (strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0|| strcmp(registro, "CX") == 0|| strcmp(registro, "DX") == 0){
        if (strcmp(operacion, "+")){
            *(uint8_t*)registro_destino += *(uint8_t*) registro_origen;
        }else if (strcmp(operacion, "-")){
            *(uint8_t*)registro_destino -= *(uint8_t*) registro_origen;
        }else if(strcmp(operacion, "set")){
            *(uint8_t*)registro_destino = valor;
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

    // Enviar el paquete a la KERNEL
    enviar_paquete(paquete_a_kernel,config_cpu->SOCKET_KERNEL);

    // Liberar recursos del paquete
    eliminar_paquete(paquete_a_kernel);
}



void tengoAlgunaInterrupcion(){
    if (atomic_load(&interrupt_flag) == 1){
    //Se recibio una interrupcion
    t_paquete* paquete_a_kernel = crear_paquete(FINQUANTUM);
    atomic_store(&interrupt_flag,0);
    enviar_pcb_a_kernel(paquete_a_kernel);
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
            seguir_ciclo(config_cpu->SOCKET_KERNEL, config_cpu->SOCKET_DISPATCH);
            break;
        case SUM:
            ejecutar_sum(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            seguir_ciclo(config_cpu->SOCKET_KERNEL, config_cpu->SOCKET_DISPATCH);
            break;
        case SUB:
            ejecutar_sub(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            seguir_ciclo(config_cpu->SOCKET_KERNEL, config_cpu->SOCKET_DISPATCH);
            break;           
        case JNZ:
            ejecutar_JNZ(instruccion->parametro1,instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            seguir_ciclo(config_cpu->SOCKET_KERNEL, config_cpu->SOCKET_DISPATCH);
            break;
        case IO_GEN_SLEEP:
           ejecutar_IO_GEN_SLEEP(instruccion->parametro1,instruccion->parametro2);
           //Tipo desalojo IO
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case MOVE_IN:
           ejecutar_MOV_IN(instruccion->parametro1, instruccion->parametro2);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            seguir_ciclo(config_cpu->SOCKET_KERNEL, config_cpu->SOCKET_DISPATCH);
            break;
        case MOV_OUT:
           ejecutar_MOV_OUT(instruccion->parametro1, instruccion->parametro2);
           log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
           seguir_ciclo(config_cpu->SOCKET_KERNEL, config_cpu->SOCKET_DISPATCH);
            break;
        case RESIZE:
            //ejecutar_MOV_OUT(tamanio);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case COPY_STRING:
           // ejecutar_COPY_STRING(tamanio);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case WAIT:
            //ejecutar_WAIT(recurso);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
            break;
        case SIGNAL:
            //ejecutar_SINGAL(recurso);
            log_info(logger,"Instruccion Ejecutada: PID: %d- Ejecutando: %s -%s %s", pcb->pid,instruccion->opcode,instruccion->parametro1,instruccion->parametro2);
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
    enviar_a_leer_memoria(mmu_mov_in->direccionFIsica, tamanio_registro); //Memoria me dijo que no hace falta el PID 
    int valor = recv_leer_memoria();

    operar_con_registros(reg_Datos,NULL,registro_Datos,"set",valor);
}

void enviar_a_leer_memoria(int direccionFIsica, int tamanio){
    t_paquete* solicitud_lectura = crear_paquete(ACCESO_A_LECTURA);
    agregar_a_paquete(solicitud_lectura, &direccionFIsica,sizeof(int));
    agregar_a_paquete(solicitud_lectura, &tamanio,sizeof(int));
    enviar_paquete(solicitud_lectura, config_cpu->SOCKET_MEMORIA);
    eliminar_paquete(solicitud_lectura);
}

int recv_leer_memoria(){
    int valor,size;
    void* buffer = recibir_buffer(&size, config_cpu->SOCKET_MEMORIA);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
        exit(-1);
    }
    memcpy(&valor, buffer, sizeof(int));
    return valor;
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

    //Noc como hacer la verificacion ver despues 
    send_escribi_memoria(mmu_mov_in->direccionFIsica, tamanio_registro, valor); //Memoria me dijo que no hace falta el PID 
    if(recv_escribir_memoria() == 1){
        log_info(logger,"Se puedo escribir correctamente");
    }else{
        log_error(logger,"No se pudo escribir en memoria");
    }
}
void send_escribi_memoria(int direccionFIsica, int tamanio,int valor){
    t_paquete* solicitud_lectura = crear_paquete(ACCESO_A_ESCRITURA);
    agregar_a_paquete(solicitud_lectura, &direccionFIsica,sizeof(int));
    agregar_a_paquete(solicitud_lectura, &tamanio,sizeof(int));
    agregar_a_paquete(solicitud_lectura, &valor,sizeof(int));
    enviar_paquete(solicitud_lectura, config_cpu->SOCKET_MEMORIA);
    eliminar_paquete(solicitud_lectura);
}

int recv_escribir_memoria(){
    int valor,size;
    void* buffer = recibir_buffer(&size, config_cpu->SOCKET_MEMORIA);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
    }
    memcpy(&valor, buffer, sizeof(int));
    return valor;
}   

t_mmu_cpu* traducirDireccion(int direccionLogica){
    t_mmu_cpu* mmu = malloc(sizeof(t_mmu_cpu));
    if (!mmu){
        log_error(logger,"Error a asignar memorua para MMU");
        return NULL;
    }
    
    mmu->num_pagina = floor(direccionLogica / config_cpu->TAMANIO_MARCO);
    mmu->ofset = direccionLogica - mmu->num_pagina * config_cpu->TAMANIO_MARCO;
    //buscar en mis paginas de la cpu
    
    t_tabla_de_paginas_cpu* tabla =  buscarEnTLB(pcb->tabla_cpu,mmu->num_pagina);
    if (tabla == NULL){
    if (list_size(pcb->tabla_cpu) >= config_cpu->CANTIDAD_ENTRADAS_TLB){   //No puede ser mayor pero lo dejo por las daudas
        if (strcmp(config_cpu->ALGORITMO_TLB ,"FIFO")){
            actualizar_lru_por_fifo(pcb->tabla_cpu, mmu->num_pagina);
        }else if (strcmp(config_cpu->ALGORITMO_TLB ,"LRU")){
            actualizar_lru_por_tlb(pcb->tabla_cpu, mmu->num_pagina);             
        }else{
            log_error(logger, "Algoritmo no es valido en mmu");
            free(mmu);
            return NULL;
        }
        }else{
            solicitar_tablas_a_memoria(mmu->num_pagina); 
            t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
            if (nueva_tabla != NULL){
                list_add(pcb->tabla_cpu,nueva_tabla);
            } 
        }
        
    }else{
        mmu->direccionFIsica = config_cpu->TAMANIO_MARCO * mmu->num_pagina + mmu->ofset;
    }
    return mmu;
}

t_tabla_de_paginas_cpu* buscarEnTLB(t_list* tablasCpu, int num_pagina){
    for (int i = 0; i < list_size(tablasCpu); i++){
        t_tabla_de_paginas_cpu* tlb = list_get(tablasCpu,i);
        if (tlb->nropagina == num_pagina){
            if (strcmp(config_cpu->ALGORITMO_TLB ,"LRU")){
                tlb->contador = 0;
            }
            return tlb;
        }
    }
    return NULL;
}
void actualizar_lru_por_fifo(t_list* tlb, int numero_pagina){
//Segunda opcion pero aca no mantengo la tabla  
    // t_tabla_de_paginas_cpu* victima = list_remove(tlb, 0);
    // free(victima);

    // solicitar_instruccion(numero_pagina);
    // t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
    // if (nueva_tabla != NULL){
    //     list_add(tlb,nueva_tabla);
    // }

     int i = 0;
        if (i <= config_cpu->CANTIDAD_ENTRADAS_TLB){
            list_remove(pcb->tabla_cpu, i);
            solicitar_tablas_a_memoria(numero_pagina); 
            t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
            if (nueva_tabla != NULL){
                list_add_in_index(pcb->tabla_cpu,i,nueva_tabla);
            }else{
                log_error(logger, "No llego nada la tabla de memoria FIFO");
            }
            i++;
        }else{
            i = 0;
            solicitar_tablas_a_memoria(numero_pagina); 
            t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
            if (nueva_tabla != NULL){
                list_add_in_index(pcb->tabla_cpu,i,nueva_tabla);
            }else{
                log_error(logger, "No llego nada la tabla de memoria FIFO");
            }
            i++;
        }        
}


void actualizar_lru_por_tlb(t_list* tlb, int numero_pagina){
    int contador = INT_MAX;
    int indice = -1;

    for (int i = 0; i < list_size(tlb); i++){
        t_tabla_de_paginas_cpu* tabla = list_get(tlb,i);
        tabla->contador ++;
    }

    for (int i = 0; i < list_size(tlb); i++){
        t_tabla_de_paginas_cpu* tabla = list_get(tlb,i);
        if (tabla->contador < contador){
            contador = tabla->contador;
            indice = i; 
        }
    }
if (indice != -1)
{
   t_tabla_de_paginas_cpu* tabla_a_eliminar = list_remove(tlb,indice);
    free(tabla_a_eliminar);

    solicitar_tablas_a_memoria(numero_pagina);
    t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
    if (nueva_tabla != NULL){
        nueva_tabla->contador = 0;
        list_add(tlb, nueva_tabla);
    }else{
        log_error(logger, "No llego nada la tabla de memoria LRU");
    }
    free(nueva_tabla);
}else{
    log_error(logger, "Error en reccorer la tlb en LRU");
}

}

void solicitar_tablas_a_memoria(int numero_pagina){

    t_paquete* paquete_tablas = crear_paquete(ACCEDER_TABLA_PAGINAS);
    agregar_a_paquete(paquete_tablas,&pcb->pid,sizeof(int));
    agregar_a_paquete(paquete_tablas,&numero_pagina,sizeof(int));

    enviar_paquete(paquete_tablas, config_cpu->SOCKET_MEMORIA);

    eliminar_paquete(paquete_tablas);
}   


t_tabla_de_paginas_cpu* recv_tablas(){
    t_tabla_de_paginas_cpu* tabla = malloc(sizeof(t_tabla_de_paginas_cpu));
    if(tabla == NULL){
        log_error(logger, "Erorr al asignar memoria para la tabla");
        free(tabla);
        return NULL;
    }

    int size;
    int desplazamiento = 0;
    
    void* buffer = recibir_buffer(&size, config_cpu->SOCKET_MEMORIA);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
        free(tabla);
        return NULL;
    }
    memcpy(&tabla->pid, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&tabla->nropagina, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&tabla->marco, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    free(buffer);
    return tabla;
}

int espacio_de_registro(char* registro){
    if (strcmp(registro, "AX") == 0 || strcmp(registro, "BX") == 0|| strcmp(registro, "CX") == 0|| strcmp(registro, "DX") == 0){
        return 8;
    }else if (strcmp(registro, "PC") == 0 || strcmp(registro, "EAX") == 0|| strcmp(registro, "EBX") == 0 || strcmp(registro, "ECX") == 0 || strcmp(registro, "EDX") == 0 || strcmp(registro, "SI") == 0|| strcmp(registro, "DI") == 0){
        return 32;
    }else{
        log_error(logger,"Error al calulcar el tamanio del registro");
        return 0;
    }
}

//Parte 2 min 47