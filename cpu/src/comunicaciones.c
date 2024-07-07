#include "comunicaciones.h"


int recv_pagina(int socket){
    recibir_operacion(socket);
    int size;
    int valor;
    void* buffer = recibir_buffer(&size, socket);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
        return -1;
        }
    memcpy(&valor, buffer, sizeof(int));
    free(buffer); // CAMBIADO
    return valor;
}



int comunicaciones_con_memoria_lectura(t_mmu_cpu* mmu){
    int valor_final = 0;
    int desplazamiento = 0;
    

   while (!list_is_empty(mmu->direccionFIsica)){
        int* direccionFIsicaa = (int*)list_remove(mmu->direccionFIsica, 0);
        int* tamanio = (int*)list_remove(mmu->tamanio, 0);

        if (!direccionFIsicaa || !tamanio) {
            log_error(logger, "Error al obtener dirección física o tamaño");
            free(direccionFIsicaa);
            free(tamanio);
            return -1;
        }

        int tam = *tamanio;
        int direc = *direccionFIsicaa;


        enviar_a_leer_memoria(pcb->pid, direc, tam);
        void* valor = recv_leer_memoria(tam);
        void* valor_ptr = &valor_final;
        if (!valor || !valor_ptr){
            free(direccionFIsicaa);
            free(tamanio);
            return -1;
        }

        // Concatenar la parte leída al valor final
        memcpy((unsigned char*)valor_ptr + desplazamiento, valor, tam);
        desplazamiento += tam;
        
        log_info(logger, "PID: %i - Acción LEER - Dirección Física: %i - Valor: %i", pcb->pid, direc, valor_final);

        free(valor);
        free(direccionFIsicaa);
        free(tamanio);


   }
    return valor_final;
}

char* comunicaciones_con_memoria_lectura_copy_string(t_mmu_cpu* mmu) {
    int desplazamiento = 0;
    int total_tam = 0;

    for (int i = 0; i < list_size(mmu->direccionFIsica); i++) {
        int* tamnio = (int*)list_get(mmu->tamanio, i);
        total_tam += *tamnio;
    }

    char* palabra = malloc(total_tam + 1);
    if (palabra == NULL) {
        log_warning(logger, "NO HAY MEMORIA TONTISSS");
        return NULL;
    }

    while (!list_is_empty(mmu->direccionFIsica)) {
        int* direccionFIsicaa = (int*)list_remove(mmu->direccionFIsica, 0);
        int* tamanio = (int*)list_remove(mmu->tamanio, 0);

        if (!direccionFIsicaa || !tamanio) {
            log_error(logger, "Error al obtener dirección física o tamaño");
            free(direccionFIsicaa);
            free(tamanio);
            free(palabra);
            return NULL;
        }

        int tam = *tamanio;
        int direc = *direccionFIsicaa;

        enviar_a_leer_memoria(pcb->pid, direc, tam);
        void* valor = recv_leer_memoria(tam);
        if (!valor) {
            free(direccionFIsicaa);
            free(tamanio);
            free(palabra);
            return NULL;
        }

        memcpy(palabra + desplazamiento, valor, tam);
        desplazamiento += tam;

        log_info(logger, "PID: %i - Acción LEER - Dirección Física: %i - Parte Leída: %s", pcb->pid, direc, (char*)valor);

        free(valor);
        free(direccionFIsicaa);
        free(tamanio);
    }

    palabra[total_tam] = '\0'; // Agregar el carácter nulo al final

    char* resultado_filtrado = filtrar_nueva_linea(palabra);
    free(palabra);

    return resultado_filtrado;
}

int comunicaciones_con_memoria_escritura_copy_string(t_mmu_cpu* mmu, char* valor){
    int verificador;
    char* palabra;
    int desplazamiento = 0;
    int des = 0;
    int longitud = strlen(valor);
    char* palbara_reconstruida = (char*)malloc(longitud + 1);

    if (palbara_reconstruida == NULL){
            log_warning(logger, "ERRORE ASIGNAR MEMORIA");
            return -1;
        }

    while (!list_is_empty(mmu->direccionFIsica)) {
        int* direccionFIsicaa = (int*)list_remove(mmu->direccionFIsica, 0);
        int* tamanio = (int*)list_remove(mmu->tamanio, 0);

        if (!direccionFIsicaa || !tamanio) {
            log_error(logger, "Error al obtener dirección física o tamaño");
            free(direccionFIsicaa);
            free(tamanio);
            free(palbara_reconstruida);
            return -1;
        }

        int tam = *tamanio;
        int direc = *direccionFIsicaa;

        // Reservar memoria para la parte del valor
        char* palabra_parte = (char*)malloc(tam);
        if (palabra_parte == NULL){
            log_warning(logger, "ERRORE ASIGNAR MEMORIA");
            free(direccionFIsicaa);
            free(tamanio);
            free(palbara_reconstruida);
            return -1;
        }
        

        // Copiar la parte del valor en la memoria reservada
        memcpy(palabra_parte, valor + desplazamiento, tam); // NOC SI VA EL (unsigned char*)&
        desplazamiento += tam;

        // Reconstruir la parte del valor dividido
        memcpy(palbara_reconstruida + des, palabra_parte, tam); // NOC SI VA EL (unsigned char*)&
        des += tam;

        send_escribi_memoria_string(pcb->pid, direc, tam, palabra_parte);
        verificador = recv_escribir_memoria();
        if (verificador == -1) {
            log_error(logger, "Error en escritura memoria, direccion fisica: %d", direc);
            t_paquete* paquete_a_kernel = crear_paquete(OUT_OF_MEMORY);
            enviar_pcb_a_kernel(paquete_a_kernel);
            enviar_paquete(paquete_a_kernel, config_cpu->SOCKET_KERNEL);
            eliminar_paquete(paquete_a_kernel);
            free(direccionFIsicaa);
            free(tamanio);
            free(palabra_parte);
            free(palbara_reconstruida);
            return -1;
        }

            log_info(logger, "PID: %i - Acción ESCRIBIR - Dirección Física: %i - Valor: %s", pcb->pid, direc, palabra_parte);
        

        free(direccionFIsicaa);
        free(tamanio);
        free(palabra_parte);
    }

    log_info(logger, "PALABRA RECONSTRUIDA: %s", palbara_reconstruida);
    free(palbara_reconstruida);
    return verificador;
    
    }

int comunicaciones_con_memoria_escritura(t_mmu_cpu* mmu, int valor) {
    int verificador = -1;
    uint32_t registro_reconstruido = 0; 
    int desplazamiento = 0;
    int des = 0;

    while (!list_is_empty(mmu->direccionFIsica)) {
        int* direccionFIsicaa = (int*)list_remove(mmu->direccionFIsica, 0);
        int* tamanio = (int*)list_remove(mmu->tamanio, 0);

        if (!direccionFIsicaa || !tamanio){
            log_error(logger, "Error al obtener direccion fisica y tamanio");
            free(direccionFIsicaa);
            free(tamanio);
            return -1;
        }
        
        int tam = *tamanio;
        int direc = *direccionFIsicaa;

        // Reservar memoria para la parte del valor
        void* registro_parte = malloc(tam);
        if (!registro_parte){
            log_error(logger, "Error al asignar memoria a registro_parte");
            free(direccionFIsicaa);
            free(tamanio);
            return -1;
        }

        // Copiar la parte del valor en la memoria reservada
        memcpy(registro_parte, (unsigned char*)&valor + desplazamiento, tam);
        desplazamiento += tam;

        // Reconstruir la parte del valor dividido
        memcpy((unsigned char*)&registro_reconstruido + des, registro_parte, tam);
        des += tam;

        // Asegúrate de que `pcb` y `config_cpu` estén correctamente inicializados y accesibles
        send_escribi_memoria(pcb->pid, direc, tam, *(uint32_t*)registro_parte);
        verificador = recv_escribir_memoria();
        if (verificador == -1) {
            log_error(logger, "Error en escritura memoria, direccion fisica: %d", direc);
            t_paquete* paquete_a_kernel = crear_paquete(OUT_OF_MEMORY);
            enviar_pcb_a_kernel(paquete_a_kernel);
            enviar_paquete(paquete_a_kernel, config_cpu->SOCKET_KERNEL);
            eliminar_paquete(paquete_a_kernel);
            free(direccionFIsicaa);
            free(tamanio);
            free(registro_parte);
            return -1;
        } 
        
        log_info(logger, "PID: %i - Acción ESCRIBIR - Dirección Física: %i - Valor: %u", pcb->pid, direc, *(uint32_t*)registro_parte);

        free(direccionFIsicaa);
        free(tamanio);
        free(registro_parte);
    }

    log_info(logger, "VALOR RECONSTRUIDO: %i", registro_reconstruido);
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
    recibir_operacion(config_cpu->SOCKET_MEMORIA);

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
    recibir_operacion(config_cpu->SOCKET_MEMORIA);
    int valor,size;
    void* buffer = recibir_buffer(&size, config_cpu->SOCKET_MEMORIA);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket");
    }
    memcpy(&valor, buffer, sizeof(int));
    free(buffer);
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
    recibir_operacion(config_cpu->SOCKET_MEMORIA);
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
    recibir_operacion(config_cpu->SOCKET_MEMORIA);
    int size;
    int estado = -1;
    void* buffer = recibir_buffer(&size, config_cpu->SOCKET_MEMORIA);
    if (buffer == NULL) {
        log_error(logger, "Error al recibir el buffer del socket en agrandar memoria");
    }
    memcpy(&estado, buffer, sizeof(int));
    free(buffer); //CAMBIADO
    return estado;
}

void send_escribi_memoria_string(int pid,int direccionFIsica, int tamanio,char* valor){
    t_paquete* solicitud_escritura = crear_paquete(ACCESO_A_ESCRITURA);
    agregar_a_paquete(solicitud_escritura, &pid ,sizeof(int));
    agregar_a_paquete(solicitud_escritura, &direccionFIsica,sizeof(int));
    agregar_a_paquete_string(solicitud_escritura,valor,strlen(valor) + 1);
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

    int respuesta;
    recv(config_cpu->SOCKET_KERNEL, &respuesta , sizeof(int), MSG_WAITALL);
    log_warning(logger, "RESPUESTA:%d", respuesta);
    if(respuesta == 1) {
        //log_info(logger, "Hola!");
        agregar_a_paquete_string(solicitar_std ,interfaz,strlen(interfaz) + 1);
        log_warning(logger, "interfaz:%s", interfaz);
        while (!list_is_empty(mmu->direccionFIsica)){
            int* direccion_fisica = list_remove(mmu->direccionFIsica, 0);
            int* ptr_tamanio = list_remove(mmu->tamanio, 0);
            int tamanio = *ptr_tamanio;
            int direc_fisica = *direccion_fisica;

            log_info(logger, "DIRECCION FISICA:%i", direc_fisica);
            log_info(logger, "TAMANIO: %i", tamanio);

            agregar_a_paquete(solicitar_std, &direc_fisica, sizeof(int));
            agregar_a_paquete(solicitar_std, &tamanio, sizeof(int));
        }

        enviar_paquete(solicitar_std, config_cpu->SOCKET_KERNEL);
        eliminar_paquete(solicitar_std);
       // mostrar_pcb(pcb);
        //log_info(logger, "Hola!");
    } else
        log_error(logger , "Erro en la respuesta de desalojo de I/O");
}

void mostrar_pcb(t_pcb_cpu* pcb){
    log_info(logger,"PID: %i", pcb->pid);
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

char* filtrar_nueva_linea(char* cadena) {
    int longitud = strlen(cadena);
    char* resultado = malloc(longitud + 1);
    if (resultado == NULL) {
        return NULL;
    }

    int j = 0;
    for (int i = 0; i < longitud; i++) {
        if (cadena[i] != '\n') {
            resultado[j++] = cadena[i];
        }
    }
    resultado[j] = '\0';
    return resultado;
}