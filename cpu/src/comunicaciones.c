#include "comunicaciones.h"


int recv_pagina(int socket){
    op_code cod_op = recibir_operacion(socket);
    int size;
    int valor;
    void* buffer = recibir_buffer(&size, socket);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
        return -1;
        }
    memcpy(&valor, buffer, sizeof(int));
    return valor;
}



char* comunicaciones_con_memoria_lectura(t_mmu_cpu* mmu){
    char* valor;
    char* nuevo_valor;
    while (list_is_empty(mmu->direccionFIsica)){
        int direccionFIsicaa = (int)(intptr_t)list_remove(mmu->direccionFIsica,0);
        int tamanio = (int)(intptr_t)list_remove(mmu->tamanio, 0);
        enviar_a_leer_memoria(pcb->pid,direccionFIsicaa, tamanio); 
        valor = recv_leer_memoria(tamanio); 
        log_info(logger,"PID: %i -Accion LEER -Direccion Fisica: %i -Valor: %s",pcb->pid,direccionFIsicaa,valor);
        strcat(nuevo_valor, valor);
    }
    return nuevo_valor;
}

int comunicaciones_con_memoria_escritura(t_mmu_cpu* mmu, char* valor){
    int verificador;
    int desplazamiento = 0;
    int valor_largo = strlen(valor);

    while (!list_is_empty(mmu->direccionFIsica)){
        int* direccionFIsicaa = (int*)list_remove(mmu->direccionFIsica,0);
        int* tamanio = (int*)list_remove(mmu->tamanio, 0);
        int tam  = *tamanio;
        int direc = *direccionFIsicaa;
        // log_warning(logger, "TAM: %i", tam);
        // log_warning(logger, "DIrec: %i", direc);

        char* fragmentacion = (char*)malloc(tam + 1);
        if(fragmentacion == NULL){
            log_error(logger, "Errro en fragmentacion del string");
            free(direccionFIsicaa);
            free(tamanio);
            return -1;
        }

        strncpy(fragmentacion, valor + desplazamiento, tam);
        //log_warning(logger, "valor: %s", fragmentacion);

        send_escribi_memoria(pcb->pid,direc, tam  , valor + desplazamiento);
        desplazamiento += tam ;
        

        verificador = recv_escribir_memoria();
        if (verificador != -1){
            log_error(logger,"Error en memoria  direccion fisica :%d", direc);
            free(direccionFIsicaa);
            free(tamanio);
            free(fragmentacion);
            return -1;
        }
        log_info(logger,"PID: %i -Accion ESCRIBIR -Direccion Fisica: %i -Valor: %s",pcb->pid,direc,valor);
    free(direccionFIsicaa);
    free(tamanio);
    free(fragmentacion);
    }

    return verificador;
}



t_pcb_cpu* rcv_contexto_ejecucion_cpu(int socket_cliente) {
    
    t_pcb_cpu* proceso = (t_pcb_cpu*)malloc(sizeof(t_pcb_cpu));
    if (proceso == NULL) {
        log_error(logger, "Error al asignar memoria para el proceso");
        return NULL;
    }

    proceso->registros = (t_registro_cpu*)malloc(sizeof(t_registro_cpu));
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
    //log_info(logger,"PID: %i", proceso->pid);

    memcpy(&proceso->program_counter, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    //log_info(logger,"Program Counter:%i",proceso->program_counter);

    memcpy(&proceso->registros->PC, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    //log_info(logger,"Reg PC:%i",proceso->registros->PC);

    memcpy(&proceso->registros->AX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    //log_info(logger,"Reg AX:%i",proceso->registros->AX);

    memcpy(&proceso->registros->BX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    //log_info(logger,"Reg BX:%i",proceso->registros->BX);

    memcpy(&proceso->registros->CX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
    //log_info(logger,"Reg CX:%i",proceso->registros->CX);

    memcpy(&proceso->registros->DX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);
   //log_info(logger,"Reg DX:%i",proceso->registros->DX);

    memcpy(&proceso->registros->EAX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    //log_info(logger,"Reg EAX:%i",proceso->registros->EAX);

    memcpy(&proceso->registros->EBX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    //log_info(logger,"Reg EBX:%i",proceso->registros->EBX);

    memcpy(&proceso->registros->ECX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    //log_info(logger,"Reg ECX:%i",proceso->registros->ECX);

    memcpy(&proceso->registros->EDX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    //log_info(logger,"Reg EDX:%i",proceso->registros->EDX);

    memcpy(&proceso->registros->SI, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    //log_info(logger,"Reg SI:%i",proceso->registros->SI);

    memcpy(&proceso->registros->DI, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
    //log_info(logger,"Reg DI:%i",proceso->registros->DI);

    free(buffer);
    return proceso;
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
}


void enviar_a_leer_memoria(int pid,int direccionFIsica, int tamanio){
    t_paquete* solicitud_lectura = crear_paquete(ACCESO_A_LECTURA);
    agregar_a_paquete(solicitud_lectura, &pid , sizeof(int));
    agregar_a_paquete(solicitud_lectura, &direccionFIsica,sizeof(int));
    agregar_a_paquete(solicitud_lectura, &tamanio,sizeof(int));
    enviar_paquete(solicitud_lectura, config_cpu->SOCKET_MEMORIA);
    eliminar_paquete(solicitud_lectura);
}

char* recv_leer_memoria(int tamanio){
    int size;
    char* valor;
    void* buffer = recibir_buffer(&size, config_cpu->SOCKET_MEMORIA);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
        exit(-1);
    }
    memcpy(&valor, buffer, tamanio);
    return valor;
}   

void send_escribi_memoria(int pid,int direccionFIsica, int tamanio,char* valor){
    t_paquete* solicitud_escritura = crear_paquete(ESCRIBIR_MEMORIA);
    agregar_a_paquete(solicitud_escritura, &pid ,sizeof(int));
    agregar_a_paquete(solicitud_escritura, &direccionFIsica,sizeof(int));
    agregar_a_paquete_string(solicitud_escritura, &valor, tamanio + 1);
    enviar_paquete(solicitud_escritura, config_cpu->SOCKET_MEMORIA);
    eliminar_paquete(solicitud_escritura);
}

int recv_escribir_memoria(){
    op_code code = recibir_operacion(config_cpu->SOCKET_MEMORIA);
    int valor,size;
    void* buffer = recibir_buffer(&size, config_cpu->SOCKET_MEMORIA);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
    }
    memcpy(&valor, buffer, sizeof(int));
    return valor;
}   

void solicitar_tablas_a_memoria(int numero_pagina){

    t_paquete* paquete_tablas = crear_paquete(ACCEDER_TABLA_PAGINAS);
    agregar_a_paquete(paquete_tablas,&pcb->pid,sizeof(int));
    agregar_a_paquete(paquete_tablas,&numero_pagina,sizeof(int));

    enviar_paquete(paquete_tablas, config_cpu->SOCKET_MEMORIA);

    eliminar_paquete(paquete_tablas);
}   


t_tabla_de_paginas_cpu* recv_tablas(){
    op_code code = recibir_operacion(config_cpu->SOCKET_MEMORIA);
    t_tabla_de_paginas_cpu* tabla = (t_tabla_de_paginas_cpu*)malloc(sizeof(t_tabla_de_paginas_cpu));
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
    // memcpy(&tabla->pid, buffer + desplazamiento, sizeof(int));
    // desplazamiento += sizeof(int);

    // memcpy(&tabla->nropagina, buffer + desplazamiento, sizeof(int));
    // desplazamiento += sizeof(int);

    memcpy(&tabla->marco, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    free(buffer);
    return tabla;
}

void send_agrandar_memoria (int pid , int tamanio){
    t_paquete* paquete_a_agrandar = crear_paquete(MODIFICAR_TAMAÑO_MEMORIA);
    agregar_a_paquete(paquete_a_agrandar, &pid, sizeof(int));
    agregar_a_paquete(paquete_a_agrandar, &tamanio, sizeof(int));
    enviar_paquete(paquete_a_agrandar,config_cpu->SOCKET_MEMORIA);
    eliminar_paquete(paquete_a_agrandar);
}

int recv_agrandar_memoria() {
    op_code code = recibir_operacion(config_cpu->SOCKET_MEMORIA);
    int size;
    int estado = -1;
    void* buffer = recibir_buffer(&size, config_cpu->SOCKET_MEMORIA);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket en agrandar memoria");
    }
    memcpy(&estado, buffer, sizeof(int));
    return estado;
}

void send_escribi_memoria_string(int pid,int direccionFIsica, int tamanio,char* valor){
    t_paquete* solicitud_escritura = crear_paquete(ACCESO_A_ESCRITURA);
    agregar_a_paquete(solicitud_escritura, &pid ,sizeof(int));
    agregar_a_paquete(solicitud_escritura, &direccionFIsica,sizeof(int));
    agregar_a_paquete(solicitud_escritura, &tamanio,sizeof(int));
    agregar_a_paquete(solicitud_escritura, &valor,sizeof(int));
    enviar_paquete(solicitud_escritura, config_cpu->SOCKET_MEMORIA);
    eliminar_paquete(solicitud_escritura);
}

char* recv_escribir_memoria_string(int tamanio){
    int size;
    char* valor;
    void* buffer = recibir_buffer(&size, config_cpu->SOCKET_MEMORIA);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
    }
    memcpy(&valor, buffer, tamanio);
    return valor;
}   

void solicitar_a_kernel_std(char* interfaz ,int tamanio, t_list* direcciones_fisicas,t_paquete* solicitar_std){
    int respuesta;
    recv(config_cpu->SOCKET_KERNEL, &respuesta , sizeof(int), MSG_WAITALL);
    if(respuesta == 1){
        t_paquete* paquete_std = crear_paquete (STDIN);
        agregar_a_paquete_string(paquete_std ,interfaz,strlen(interfaz) + 1);
        agregar_a_paquete(paquete_std, &tamanio, sizeof(int));
    while (list_is_empty(direcciones_fisicas)){
        int* direccion_fisica = (int*)list_remove(direcciones_fisicas, 0);
        agregar_a_paquete(paquete_std, direccion_fisica, sizeof(int));
        free(direccion_fisica);
    }
    enviar_paquete(paquete_std, config_cpu->SOCKET_KERNEL);
    eliminar_paquete(paquete_std);
    }else{log_error(logger , "Erro en la respuesta de desalojo de I/O");}
}

void mostrar_pcb(t_pcb_cpu* pcb){
    log_info(logger,"PID: %i", pcb->pid);
    log_info(logger,"Program Counter:%i",pcb->program_counter);
    log_info(logger,"Reg PC:%i",pcb->registros->PC);
    log_info(logger,"Reg AX:%i",pcb->registros->AX);
    log_info(logger,"Reg BX:%i",pcb->registros->BX);
    log_info(logger,"Reg CX:%i",pcb->registros->CX);
    log_info(logger,"Reg DX:%i",pcb->registros->DX);
    log_info(logger,"Reg EAX:%i",pcb->registros->EAX);
    log_info(logger,"Reg EBX:%i",pcb->registros->EBX);
    log_info(logger,"Reg ECX:%i",pcb->registros->ECX);
    log_info(logger,"Reg EDX:%i",pcb->registros->EDX);
    log_info(logger,"Reg SI:%i",pcb->registros->SI);
    log_info(logger,"Reg DI:%i",pcb->registros->DI);
}