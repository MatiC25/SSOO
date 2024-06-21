#include "mmu.h"
int posicion_fifo = 0;


t_mmu_cpu* traducirDireccion(int direccionLogica , int tamanio){
    t_mmu_cpu* mmu = (t_mmu_cpu*)malloc(sizeof(t_mmu_cpu));
    if (!mmu){
        log_error(logger,"Error a asignar memorua para MMU");
        return NULL;
    }
    //log_info(logger, "Se inicializan las listas");
    mmu->tamanio = list_create();
    mmu->direccionFIsica = list_create();
    mmu->num_pagina = list_create();


    int pagina = (int)floor((double)direccionLogica / config_cpu->TAMANIO_PAGINA);
    //log_info(logger, "PAgina: %i", pagina);
    int*pagina_ptr = malloc(sizeof(int));
    *pagina_ptr = pagina;
    list_add(mmu->num_pagina , pagina_ptr); //Primera posicion
    //log_info(logger, "DIreccion logica: %i", direccionLogica);

    mmu->ofset = direccionLogica - pagina * config_cpu->TAMANIO_PAGINA ;
    ;//log_info(logger,"Ofset: %i",mmu->ofset);

    //int tamanioo = (int)floor((double)tamanio/config_cpu->TAMANIO_PAGINA); 
    
    int tamanioo = config_cpu->TAMANIO_PAGINA - mmu->ofset;
    int* tamanio_ptr = malloc(sizeof(int));
    *tamanio_ptr = tamanioo; 
    list_add(mmu->tamanio, tamanio_ptr);


    int direccionLogica_actualizada = (pagina + 1) * config_cpu->CANTIDAD_ENTRADAS_TLB;

    tamanioo = tamanio - tamanioo;


    while (config_cpu ->TAMANIO_PAGINA <= tamanioo){ 
        int pagina_actualizada = (int)floor((double)direccionLogica_actualizada / config_cpu->TAMANIO_PAGINA);
        log_info(logger, "Nro pagina actualizada: %i", pagina_actualizada);
        int* pagina_ptr_nuevo = malloc(sizeof(int));
        *pagina_ptr_nuevo = pagina_actualizada; //Posicion N
        list_add(mmu->num_pagina , pagina_ptr_nuevo);

         direccionLogica_actualizada = pagina_actualizada * config_cpu->CANTIDAD_ENTRADAS_TLB;
         tamanioo -= config_cpu -> TAMANIO_PAGINA;

        int* tamanio_ptr_nuevo = malloc(sizeof(int));
        *tamanio_ptr_nuevo = tamanioo;
        list_add(mmu->tamanio, tamanio_ptr_nuevo);
    }

    if (tamanioo  > 0){
        //Ultima posicion
         int ultima_pagina = (int)floor((double)direccionLogica_actualizada / config_cpu->TAMANIO_PAGINA);
        int* pagina_ptr_ultima = malloc(sizeof(int));
        *pagina_ptr_ultima = ultima_pagina;
        list_add(mmu->num_pagina , pagina_ptr_ultima);

        int* tamanio_ptr_ultimo = malloc(sizeof(int));
        *tamanio_ptr_ultimo = tamanioo;
        list_add(mmu->tamanio, tamanio_ptr_ultimo);
    }


for (int i = 0; i < list_size(mmu->num_pagina); i++){
    int* pagina = (int*)list_get(mmu->num_pagina, i);
    int pagina_a_buscar = *pagina;
    //log_info(logger, "pagina_a_buscar: %i", pagina_a_buscar);

    t_tabla_de_paginas_cpu* tabla = (t_tabla_de_paginas_cpu*)buscarEnTLB(tlb, pagina_a_buscar);
        if (tabla == NULL){ 

        if (list_size(tlb) < config_cpu->CANTIDAD_ENTRADAS_TLB){

            solicitar_tablas_a_memoria(pagina_a_buscar);
            tabla = recv_tablas();
            tabla->pid = pcb->pid;
            tabla->contador = 0;
            tabla->nropagina = pagina_a_buscar;
            // log_warning(logger, "Marco: %i", tabla->marco);
            // log_warning(logger, "Tamanio PAgina: %i", config_cpu -> TAMANIO_PAGINA);
            // log_warning(logger, "Ofset: %i",  mmu->ofset);


            list_add(tlb,tabla);

            int dirc_fisica = tabla->marco * config_cpu -> TAMANIO_PAGINA + mmu->ofset;

            int* ptr_dirc_fisica = malloc(sizeof(int));
            if (ptr_dirc_fisica == NULL) {
                log_error(logger, "Error al asignar memoria para ptr_dirc_fisica");
                return NULL;
            }

            *ptr_dirc_fisica = dirc_fisica;
            list_add(mmu->direccionFIsica , ptr_dirc_fisica);



        }else{
        if (strcmp(config_cpu->ALGORITMO_TLB ,"FIFO") == 0){
           log_info(logger, "Entro a FIFO");
            tabla = (t_tabla_de_paginas_cpu*)actualizar_TLB_por_fifo(tlb , pagina_a_buscar);

            int dirc_fisica = tabla->marco * config_cpu -> TAMANIO_PAGINA + mmu->ofset;

            int* ptr_dirc_fisica = malloc(sizeof(int));
            if (ptr_dirc_fisica == NULL) {
                log_error(logger, "Error al asignar memoria para ptr_dirc_fisica");
                return NULL;
            }

            *ptr_dirc_fisica = dirc_fisica;
            list_add(mmu->direccionFIsica , ptr_dirc_fisica);
            free(ptr_dirc_fisica);

        }else if (strcmp(config_cpu->ALGORITMO_TLB ,"LRU") == 0){

            log_info(logger, "Entro a LRU");
             tabla = (t_tabla_de_paginas_cpu*)actualizar_Tlb_por_lru(tlb , pagina_a_buscar);      
    
            int dirc_fisica = tabla->marco * config_cpu -> TAMANIO_PAGINA + mmu->ofset;

            int* ptr_dirc_fisica = malloc(sizeof(int));
            if (ptr_dirc_fisica == NULL) {
                log_error(logger, "Error al asignar memoria para ptr_dirc_fisica");
                return NULL;
            }

            *ptr_dirc_fisica = dirc_fisica;

            list_add(mmu->direccionFIsica , ptr_dirc_fisica);
            free(ptr_dirc_fisica);
            
        }else{
            log_error(logger, "Algoritmo no es valido en mmu");
            free(mmu);
            return NULL;
        }
        }
    }else{
        int dirc_fisica = tabla->marco * config_cpu -> TAMANIO_PAGINA + mmu->ofset;

            int* ptr_dirc_fisica = malloc(sizeof(int));
            if (ptr_dirc_fisica == NULL) {
                log_error(logger, "Error al asignar memoria para ptr_dirc_fisica");
                return NULL;
            }

            *ptr_dirc_fisica = dirc_fisica;

        log_info(logger,"PID %i - TLB HIT - Pagina: %i",pcb->pid, tabla->nropagina);
        log_info(logger, "PID %i -OBTENER MARCO - Pagina: %i -Marco: %i",pcb->pid,tabla->nropagina,tabla->marco);
        list_add(mmu->direccionFIsica , ptr_dirc_fisica);
        
        free(ptr_dirc_fisica);

    }
    free(tabla);
}
    
    //free(ptr_dirc_fisica);
    return mmu;
    
}

t_tabla_de_paginas_cpu* buscarEnTLB(t_list* tablasCpu, int num_pagina){

    if(strcmp(config_cpu->ALGORITMO_TLB ,"LRU") == 0){
    for (int i = 0; i < list_size(tablasCpu); i++){ //Poder cambiar en un futuro
        t_tabla_de_paginas_cpu* tlb = list_get(tablasCpu,i);
        tlb->contador++;
//            free(tlb);
            }
    }

    for (int i = 0; i < list_size(tablasCpu); i++){
        t_tabla_de_paginas_cpu* tlb = (t_tabla_de_paginas_cpu*)list_get(tablasCpu,i);
        if (tlb->nropagina == num_pagina){
            if (strcmp(config_cpu->ALGORITMO_TLB ,"LRU") == 0 ){
                tlb->contador = 0;
            }
            return tlb;
//            free(tlb);
        }
    }
    return NULL;
}
t_tabla_de_paginas_cpu* actualizar_TLB_por_fifo(t_list* tlb ,int numero_pagina){
//Segunda opcion pero aca no mantengo la tabla  
    // t_tabla_de_paginas_cpu* victima = list_remove(tlb, 0);
    // free(victima);

    // solicitar_tablas_a_memoria(numero_pagina);
    // t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
    // if (nueva_tabla != NULL){
    //     list_add(tlb,nueva_tabla);
    // }
        
        if (posicion_fifo <= config_cpu->CANTIDAD_ENTRADAS_TLB){
            list_remove(tlb, posicion_fifo); //Puede ser tlb 
            solicitar_tablas_a_memoria(numero_pagina); 
            t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
            if (nueva_tabla != NULL){
                nueva_tabla->pid = pcb->pid;
                nueva_tabla->nropagina = numero_pagina;
                log_info(logger,"PID %i -TLB MISS -Pagina %i",pcb->pid,nueva_tabla->nropagina);
                list_add_in_index(tlb,posicion_fifo,nueva_tabla);//Puede ser tlb
                return nueva_tabla;
                free(nueva_tabla);
            }else{
                log_error(logger, "No llego nada la tabla de memoria FIFO");
                return NULL;
                free(nueva_tabla);
            }
            posicion_fifo++;
        }else{
            posicion_fifo = 0;
            solicitar_tablas_a_memoria(numero_pagina); 
            t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
            if (nueva_tabla != NULL){
                nueva_tabla->pid = pcb->pid;
                nueva_tabla->nropagina = numero_pagina;
                log_info(logger,"PID %i -TLB MISS -Pagina %i",pcb->pid,nueva_tabla->nropagina);
                list_add_in_index(tlb,posicion_fifo,nueva_tabla);//Puede ser tlb
                return nueva_tabla;
                free(nueva_tabla);
            }else{
                log_error(logger, "No llego nada la tabla de memoria FIFO");
                free(nueva_tabla);
                return NULL;
            }
            posicion_fifo++;
        }        
        
}


t_tabla_de_paginas_cpu* actualizar_Tlb_por_lru(t_list* tlb, int numero_pagina){
    int contador = INT_MAX;
    int indice = -1;

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
        log_info(logger,"PID %i -TLB MISS -Pagina %i",pcb->pid,nueva_tabla->nropagina);
        nueva_tabla->contador = 0;
        //list_add(tlb, nueva_tabla); //Puede ser tlb
        return nueva_tabla;
        free(nueva_tabla);
    }else{
        log_error(logger, "No llego nada la tabla de memoria LRU");
        return NULL;
        free(nueva_tabla);
    }
    
}else{
    log_error(logger, "Error en reccorer la tlb en LRU");
    return NULL;
}

}

