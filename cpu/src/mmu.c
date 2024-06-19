#include "mmu.h"
int posicion_fifo = 0;

t_mmu_cpu* traducirDireccion(int direccionLogica , int tamanio){
    t_mmu_cpu* mmu = (t_mmu_cpu*)malloc(sizeof(t_mmu_cpu));
    if (!mmu){
        log_error(logger,"Error a asignar memorua para MMU");
        return NULL;
    }
    log_info(logger, "Se inicializan las listas");
    mmu->tamanio = list_create();
    mmu->direccionFIsica = list_create();
    mmu->num_pagina = list_create();


    int pagina = (int)floor((double)direccionLogica / config_cpu->TAMANIO_PAGINA);
    log_info(logger, "Nro pagina: %i", pagina);
    list_add(mmu->num_pagina , (void*)(intptr_t)pagina); //Primera posicion
    
    mmu->ofset = direccionLogica - ((intptr_t)mmu->num_pagina * config_cpu->TAMANIO_PAGINA);

    int tamanioo = (int)floor((double)tamanio/config_cpu->TAMANIO_PAGINA); 
    log_info(logger, "Tamanio: %i", tamanio);
    list_add(mmu->tamanio, (void*)(intptr_t)tamanioo);
 

    int direccionLogica_actualizada = direccionLogica;
    log_info(logger, "DireccionesLogica: %i", direccionLogica_actualizada);
    direccionLogica_actualizada += (config_cpu->TAMANIO_PAGINA - mmu->ofset);

    tamanio -= (config_cpu->TAMANIO_PAGINA - mmu->ofset);
    

    while (config_cpu ->TAMANIO_PAGINA <= tamanio){ 
        int pagina_actualizada = (int)floor((double)direccionLogica_actualizada / config_cpu->TAMANIO_PAGINA);
        log_info(logger, "Nro pagina actualizada: %i", pagina_actualizada);
         list_add(mmu->num_pagina , (void*)(intptr_t)pagina_actualizada); //Posiciones N
         direccionLogica_actualizada += config_cpu -> TAMANIO_PAGINA;
         tamanio -= config_cpu -> TAMANIO_PAGINA;
         list_add(mmu->tamanio, (void*)(intptr_t)config_cpu->TAMANIO_PAGINA);
    }

    if (tamanio > 0){
        int ultima_pagina = (int)floor((double)direccionLogica_actualizada / config_cpu->TAMANIO_PAGINA);
        log_info(logger, "Nro pagina actualizada: %i", ultima_pagina);
        list_add(mmu->num_pagina ,(void*)(intptr_t)ultima_pagina); //Ultima pagina
        list_add(mmu->tamanio, (void*)(intptr_t)tamanio);
    }
    
for (int i = 0; i < list_size(mmu->num_pagina); i++){
    t_tabla_de_paginas_cpu* tabla = buscarEnTLB(pcb->tabla_cpu, (intptr_t)list_get(mmu->num_pagina, i));

        if (tabla == NULL){
    if (list_size(pcb->tabla_cpu) >= config_cpu->CANTIDAD_ENTRADAS_TLB){   //No puede ser mayor pero lo dejo por las daudas
    
    // solicitar_tablas_a_memoria((intptr_t)list_get(mmu->num_pagina, i)); 
    //         tabla = recv_tablas();
    //         if (tabla != NULL){
    //             log_error(logger, "Eror en recibir tabla de memeoria ");
    //         } 
        }
        if (strcmp(config_cpu->ALGORITMO_TLB ,"FIFO")){
           log_info(logger, "Entro a FIFO");
            t_tabla_de_paginas_cpu* tabla = actualizar_TLB_por_fifo(pcb->tabla_cpu , (intptr_t)list_get(mmu->num_pagina, i));
            if (tabla != NULL){
                log_error(logger, "Eror en recibir tabla de memeoria ");
            } 
            list_add(mmu->direccionFIsica , (void*)(intptr_t) (tabla->marco * config_cpu -> TAMANIO_PAGINA + mmu->ofset));
            free(tabla);
        }else if (strcmp(config_cpu->ALGORITMO_TLB ,"LRU")){
            log_info(logger, "Entro a LRU");
            t_tabla_de_paginas_cpu* tabla = actualizar_Tlb_por_lru(pcb->tabla_cpu , (intptr_t)list_get(mmu->num_pagina, i));      
              if (tabla != NULL){
                log_error(logger, "Eror en recibir tabla de memeoria ");
            } 
            list_add(mmu->direccionFIsica , (void*)(intptr_t) (tabla->marco * config_cpu -> TAMANIO_PAGINA + mmu->ofset));
            free(tabla);
        }else{
            log_error(logger, "Algoritmo no es valido en mmu");
            free(mmu);
            return NULL;
        }
    }
        log_info(logger,"PID %i - TLB HIT - Pagina: %i",pcb->pid,tabla->nropagina);
        log_info(logger, "PID %i -OBTENER MARCO - Pagina: %i -Marco: %i",pcb->pid,tabla->nropagina,tabla->marco);
        list_add(mmu->direccionFIsica , (void*)(intptr_t) (tabla->marco * config_cpu -> TAMANIO_PAGINA + mmu->ofset));
        free(tabla);
}

    
    return mmu;
}

t_tabla_de_paginas_cpu* buscarEnTLB(t_list* tablasCpu, int num_pagina){
    if(strcmp(config_cpu->ALGORITMO_TLB ,"LRU")){
    for (int i = 0; i < list_size(tablasCpu); i++){ //Poder cambiar en un futuro
        t_tabla_de_paginas_cpu* tlb = list_get(tablasCpu,i);
        tlb->contador++;
        free(tlb); //Noc si esta mal el free
            }
    }

    for (int i = 0; i < list_size(tablasCpu); i++){
        t_tabla_de_paginas_cpu* tlb = list_get(tablasCpu,i);
        if (tlb->nropagina == num_pagina){
            if (strcmp(config_cpu->ALGORITMO_TLB ,"LRU")){
                tlb->contador = 0;
            }
            return tlb;
            free(tlb);
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
            list_remove(tlb, posicion_fifo); //Puede ser pcb->tabla_cpu 
            solicitar_tablas_a_memoria(numero_pagina); 
            t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
            if (nueva_tabla != NULL){
                log_info(logger,"PID %i -TLB MISS -Pagina %i",pcb->pid,nueva_tabla->nropagina);
                //list_add_in_index(tlb,posicion_fifo,nueva_tabla);//Puede ser pcb->tabla_cpu
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
                log_info(logger,"PID %i -TLB MISS -Pagina %i",pcb->pid,nueva_tabla->nropagina);
                //list_add_in_index(tlb,posicion_fifo,nueva_tabla); //Puede ser pcb->tabla_cpu
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
        //list_add(tlb, nueva_tabla); //Puede ser pcb->tabla_cpu
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