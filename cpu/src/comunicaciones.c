#include "comunicaciones.h"

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


void enviar_a_leer_memoria(int pid,int direccionFIsica, int tamanio){
    t_paquete* solicitud_lectura = crear_paquete(ACCESO_A_LECTURA);
    agregar_a_paquete(solicitud_lectura, &pid , sizeof(int));
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

void send_escribi_memoria(int pid,int direccionFIsica, int tamanio,int valor){
    t_paquete* solicitud_escritura = crear_paquete(ACCESO_A_ESCRITURA);
    agregar_a_paquete(solicitud_escritura, &pid ,sizeof(int));
    agregar_a_paquete(solicitud_escritura, &direccionFIsica,sizeof(int));
    agregar_a_paquete(solicitud_escritura, &tamanio,sizeof(int));
    agregar_a_paquete(solicitud_escritura, &valor,sizeof(int));
    enviar_paquete(solicitud_escritura, config_cpu->SOCKET_MEMORIA);
    eliminar_paquete(solicitud_escritura);
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

void solicitar_tablas_a_memoria(int numero_pagina){

    t_paquete* paquete_tablas = crear_paquete(ACCEDER_TABLA_PAGINAS);
    agregar_a_paquete(paquete_tablas,&pcb->pid,sizeof(int));
    agregar_a_paquete(paquete_tablas,&numero_pagina,sizeof(int));

    enviar_paquete(paquete_tablas, config_cpu->SOCKET_MEMORIA);

    eliminar_paquete(paquete_tablas);
}   


t_tabla_de_paginas_cpu* recv_tablas(){
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
    memcpy(&tabla->pid, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&tabla->nropagina, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&tabla->marco, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    free(buffer);
    return tabla;
}

void send_agrandar_memoria (int pid , int tamanio){
    t_paquete* paquete_a_agrandar = crear_paquete(AMPLIACION_MEMORIA);
    agregar_a_paquete(paquete_a_agrandar, &pid, sizeof(int));
    agregar_a_paquete(paquete_a_agrandar, &tamanio, sizeof(int));
    enviar_paquete(paquete_a_agrandar,config_cpu->SOCKET_MEMORIA);
    eliminar_paquete(paquete_a_agrandar);
}

int recv_agrandar_memoria() {
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