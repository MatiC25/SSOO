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



int comunicaciones_con_memoria_lectura(t_mmu_cpu* mmu){
    int valor_final;
    int desplazamiento = 0;
    int total_tam = 0;
    int direc;

   while (!list_is_empty(mmu->direccionFIsica)){
     int* direccionFIsicaa = (int*)list_remove(mmu->direccionFIsica, 0);
        int* tamanio = (int*)list_remove(mmu->tamanio, 0);
        int tam = *tamanio;
        direc = *direccionFIsicaa;


        enviar_a_leer_memoria(pcb->pid, direc, tam);
        void* valor = recv_leer_memoria(tam);

        // Concatenar la parte leída al valor final
        memcpy((unsigned char*)&valor_final + desplazamiento, valor, tam);
        desplazamiento += tam;
        
        log_info(logger, "PID: %i - Acción LEER - Dirección Física: %i - Valor: 0x%x", pcb->pid, direc, valor_final);

        free(valor);
        free(direccionFIsicaa);
        free(tamanio);

        int verificador = recv_escribir_memoria();
        if (verificador = -1){
        t_paquete* paquete_a_kernel = crear_paquete(OUT_OF_MEMORY);
        enviar_pcb_a_kernel(paquete_a_kernel);
        enviar_paquete(paquete_a_kernel, config_cpu->SOCKET_KERNEL);
        eliminar_paquete(paquete_a_kernel);
        }
        
   }
    return valor_final;
}
 
       

int comunicaciones_con_memoria_escritura(t_mmu_cpu* mmu, int valor){
    int verificador;
    int desplazamiento = 0;
    uint32_t registro_reconstruido = 0; 
//log_warning(logger, "Cantidad de DF: %i", list_size(mmu->direccionFIsica));
    //log_info(logger,"VALOR INICIAL: %i" , valor);
    while (!list_is_empty(mmu->direccionFIsica)){
        int* direccionFIsicaa = (int*)list_remove(mmu->direccionFIsica,0);
        int* tamanio = (int*)list_remove(mmu->tamanio, 0);

        uint32_t registro_ecx = valor;
        int tam  = *tamanio;
        int direc = *direccionFIsicaa;
        int tam1 = tam;
        int desplazamiento = 0;
        int des = 0;
    
        //reservar memoria para la primera parte del valor
        void* registro_parte_1 = malloc(tam1); 
    
        // creamos los punteros a al registro original y al que reconstruiremos
        void* registro_puntero = &registro_ecx; 
        void* registro_reconstruido_puntero = &registro_reconstruido;
    
        // copia la parte del valor en la parte de momeria recervada
        memcpy(registro_parte_1, registro_puntero + desplazamiento, tam1);
        desplazamiento += tam1;
    
        //log_info( logger,"El valor de ECX completo es: %d \n", registro_ecx);
        //log_info( logger,"El valor de ECX antes de reconstruirlo: %d \n", registro_reconstruido);
    
        // esto ya serai para la parte de leer
        // reconstruir la primera parte del valor dividido
        uint32_t* medio_parte_1 = (uint32_t*)registro_parte_1;

        //log_info( logger,"El valor de la primer mitad de ECX: %d \n", *medio_parte_1);
        memcpy(registro_reconstruido_puntero +des, registro_parte_1, tam);
        des += tam;


        send_escribi_memoria(pcb->pid, direc, tam  , *medio_parte_1);
        verificador = recv_escribir_memoria();
        if (verificador == -1){
            log_error(logger,"Error en  escritura memoria  direccion fisica :%d", direc);
            //log_info(logger,"la pagina que busco no existe");
            t_paquete* paquete_a_kernel = crear_paquete(OUT_OF_MEMORY);
            enviar_pcb_a_kernel(paquete_a_kernel);
            enviar_paquete(paquete_a_kernel, config_cpu->SOCKET_KERNEL);
            eliminar_paquete(paquete_a_kernel);
            free(direccionFIsicaa);
            free(tamanio);
            return -1;
        }else{
        log_info(logger,"PID: %i -Accion ESCRIBIR -Direccion Fisica: %i -Valor: %i",pcb->pid,direc, *medio_parte_1);
        free(registro_parte_1);
        }
    free(direccionFIsicaa);
    free(tamanio);
    }
    log_info(logger,"VALOR RECONSTRUIDO: %i" , registro_reconstruido);

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
    log_warning(logger, "PID:%i", pid);
    agregar_a_paquete(solicitud_lectura, &direccionFIsica,sizeof(int));
    log_warning(logger, "DIRECCION FISICA:%i", direccionFIsica);
    agregar_a_paquete(solicitud_lectura, &tamanio,sizeof(int));
    log_warning(logger, "TAMAÑO DE LECTURA:%i", tamanio);
    enviar_paquete(solicitud_lectura, config_cpu->SOCKET_MEMORIA);
    eliminar_paquete(solicitud_lectura);
}

void* recv_leer_memoria(int tamanio){
    op_code code = recibir_operacion(config_cpu->SOCKET_MEMORIA);
    log_info(logger, "op_code: %i", code);
    int size;

    void* buffer = recibir_buffer(&size, config_cpu->SOCKET_MEMORIA);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
        exit(-1);
    }   
    log_info(logger, "size: %i", size);

    void* valor = malloc(tamanio);
    memcpy(valor, buffer, tamanio);
    free(buffer);
    return valor;
}   

void send_escribi_memoria(int pid, int direccionFIsica, int tamanio, int valor){
    t_paquete* solicitud_escritura = crear_paquete(ACCESO_A_ESCRITURA);
    agregar_a_paquete(solicitud_escritura, &pid ,sizeof(int));
    agregar_a_paquete(solicitud_escritura, &direccionFIsica,sizeof(int));
    agregar_a_paquete(solicitud_escritura, &tamanio, sizeof(int));
    agregar_a_paquete(solicitud_escritura, &valor,sizeof(int));
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
    //log_warning(logger, "PID: %i", pcb->pid);
    agregar_a_paquete(paquete_tablas,&numero_pagina,sizeof(int));
    //log_warning(logger, "Nro PAgina: %i", numero_pagina);
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

void solicitar_a_kernel_std(char* interfaz , t_mmu_cpu* mmu ,t_paquete* solicitar_std){

    int i = 0;
    int respuesta;
    recv(config_cpu->SOCKET_KERNEL, &respuesta , sizeof(int), MSG_WAITALL);

    if(respuesta == 1) {
        log_info(logger, "Hola!");
        agregar_a_paquete_string(solicitar_std ,interfaz,strlen(interfaz) + 1);

        while (!list_is_empty(mmu->direccionFIsica)){
            int* direccion_fisica = list_remove(mmu->direccionFIsica, 0);
            int* ptr_tamanio = list_remove(mmu->tamanio, 0);
            int tamanio = *ptr_tamanio;
            int direc_fisica = *direccion_fisica;

            agregar_a_paquete(solicitar_std, &direc_fisica, sizeof(int));
            agregar_a_paquete(solicitar_std, &tamanio, sizeof(int));
        }

        enviar_paquete(solicitar_std, config_cpu->SOCKET_KERNEL);
        eliminar_paquete(solicitar_std);

        log_info(logger, "Hola!");
    } else
        log_error(logger , "Erro en la respuesta de desalojo de I/O");
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