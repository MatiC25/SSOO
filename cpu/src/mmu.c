#include "mmu.h"
int posicion_fifo = 0;


t_mmu_cpu* traducirDireccion(int direccionLogica , int tamanio){
    t_mmu_cpu* mmu = (t_mmu_cpu*)malloc(sizeof(t_mmu_cpu));
    if (!mmu){
        log_error(logger,"Error a asignar memorua para MMU");
        return NULL;
    }
    //log_info(logger, "Se inicializan las listas");
    mmu->tamanio = list_create(); // LINEA 12
    mmu->direccionFIsica = list_create();// LINEA 13
    mmu->num_pagina = list_create();// LINEA 14
    mmu->ofset = list_create();


    int pagina = (int)floor((double)direccionLogica / config_cpu->TAMANIO_PAGINA);
    int offset = direccionLogica - pagina * config_cpu->TAMANIO_PAGINA ;

    int *pagina_ptr = malloc(sizeof(int));
    int *offset_ptr = malloc(sizeof(int));
    int* tamanio_ptr;
    //log_info(logger, "PAgina: %i", pagina);
    
    *pagina_ptr = pagina;
    *offset_ptr = offset;
    list_add(mmu->num_pagina , pagina_ptr); //Primera posicion
    list_add(mmu->ofset, offset_ptr);


    
    int tamanio_Actualizado = config_cpu->TAMANIO_PAGINA - offset;
    if (tamanio_Actualizado < tamanio){
    tamanio_ptr = malloc(sizeof(int));
        if (!tamanio_ptr) {
        log_error(logger, "Error al asignar memoria para tamaño");
        liberar_mmu(mmu);
        return NULL;
    }
    
    *tamanio_ptr = tamanio_Actualizado; 
    list_add(mmu->tamanio, tamanio_ptr);
    }else{
        tamanio_ptr= malloc(sizeof(int));
        *tamanio_ptr = tamanio;
        list_add(mmu->tamanio, tamanio_ptr);
        //log_info(logger,"TAMANIO: %i",*ptro);
    }
    

    int direccionLogica_actualizada = (pagina + 1) * config_cpu->TAMANIO_PAGINA;
    tamanio_Actualizado = tamanio - tamanio_Actualizado;

    while (config_cpu ->TAMANIO_PAGINA <= tamanio_Actualizado){ 
        pagina_ptr = malloc(sizeof(int));
        offset_ptr = malloc(sizeof(int));
        tamanio_ptr = malloc(sizeof(int));

        *pagina_ptr = pagina;
        *offset_ptr = 0;
        *tamanio_ptr = config_cpu->TAMANIO_PAGINA;

        list_add(mmu->num_pagina, pagina_ptr);
        list_add(mmu->ofset, offset_ptr);
        list_add(mmu->tamanio, tamanio_ptr);

        direccionLogica_actualizada += config_cpu->TAMANIO_PAGINA;
        tamanio_Actualizado -= config_cpu->TAMANIO_PAGINA;
        pagina++;
    }

    if (tamanio_Actualizado  > 0){

        pagina_ptr = malloc(sizeof(int));
        offset_ptr = malloc(sizeof(int));
        tamanio_ptr = malloc(sizeof(int));

        *pagina_ptr = pagina;
        *offset_ptr = 0;
        *tamanio_ptr = tamanio_Actualizado;

        list_add(mmu->num_pagina, pagina_ptr);
        list_add(mmu->ofset, offset_ptr);
        list_add(mmu->tamanio, tamanio_ptr);
    }


for (int i = 0; i < list_size(mmu->num_pagina); i++) {
    int* pagina = (int*)list_get(mmu->num_pagina, i);
    int* offset_ptr = (int*)list_get(mmu->ofset, i);
    int offset = *offset_ptr;
    int pagina_a_buscar = *pagina;


    t_tabla_de_paginas_cpu* tabla = (t_tabla_de_paginas_cpu*)buscarEnTLB(pagina_a_buscar);
        
    if (tabla == NULL) { // NO TLB
        
        if (list_size(tlb) < config_cpu->CANTIDAD_ENTRADAS_TLB || config_cpu->CANTIDAD_ENTRADAS_TLB == 0 ) {
            
            solicitar_tablas_a_memoria(pagina_a_buscar);
            t_tabla_de_paginas_cpu* tab = recv_tablas();
            tab->pid = pcb->pid;
            tab->contador = 0;
            tab->nropagina = pagina_a_buscar;
            
            if (list_size(tlb) < config_cpu->CANTIDAD_ENTRADAS_TLB){
                list_add(tlb, tab);
            }
             
            int dirc_fisica = tab->marco * config_cpu->TAMANIO_PAGINA + offset;

            int* ptr_dirc_fisica = malloc(sizeof(int));
            if (ptr_dirc_fisica == NULL) {
                log_error(logger, "Error al asignar memoria para ptr_dirc_fisica");
                return NULL;
            }

            *ptr_dirc_fisica = dirc_fisica;
            list_add(mmu->direccionFIsica, ptr_dirc_fisica);
            log_info(logger,"PID %i -TLB MISS -Pagina %i",pcb->pid,tab->nropagina);

        } else {
            if (strcmp(config_cpu->ALGORITMO_TLB, "FIFO") == 0) {
                //log_leo(logger2, "ENTRO A FIFO");
                t_tabla_de_paginas_cpu* tab = (t_tabla_de_paginas_cpu*)actualizar_TLB_por_fifo(pagina_a_buscar);
                log_warning(logger, "CANTIDAD DE TLB: %i", list_size(tlb));
                int dirc_fisica = tab->marco * config_cpu->TAMANIO_PAGINA + offset;

                int* ptr_dirc_fisica = malloc(sizeof(int));
                if (ptr_dirc_fisica == NULL) {
                    log_error(logger, "Error al asignar memoria para ptr_dirc_fisica");
                    return NULL;
                }

                *ptr_dirc_fisica = dirc_fisica;
                list_add(mmu->direccionFIsica, ptr_dirc_fisica);

            } else if (strcmp(config_cpu->ALGORITMO_TLB, "LRU") == 0) {
                //log_nico(logger2, "ENTRO A LRU");
                t_tabla_de_paginas_cpu* tab = (t_tabla_de_paginas_cpu*)actualizar_Tlb_por_lru(pagina_a_buscar);      
                log_nico(logger2, "CANTIDAD DE TLB DESPUES DE LRU: %i", list_size(tlb));
                
                int dirc_fisica = tab->marco * config_cpu->TAMANIO_PAGINA + offset;

                int* ptr_dirc_fisica = malloc(sizeof(int));
                if (ptr_dirc_fisica == NULL) {
                    log_error(logger, "Error al asignar memoria para ptr_dirc_fisica");
                    return NULL;
                }

                *ptr_dirc_fisica = dirc_fisica;
                list_add(mmu->direccionFIsica, ptr_dirc_fisica);
                
            } else {
                log_error(logger, "Algoritmo no es valido en mmu");
                liberar_mmu(mmu);
                return NULL;
            }
        }
    } else {
        int dirc_fisica = tabla->marco * config_cpu->TAMANIO_PAGINA + offset;
        
        int* ptr_dirc_fisica = malloc(sizeof(int));
        *ptr_dirc_fisica = dirc_fisica;

        log_info(logger, "PID %i - TLB HIT - Pagina: %i", pcb->pid, tabla->nropagina);
        log_info(logger, "PID %i - OBTENER MARCO - Pagina: %i - Marco: %i", pcb->pid, tabla->nropagina, tabla->marco);
        
        list_add(mmu->direccionFIsica, ptr_dirc_fisica);
    }
}
//free(mmu->num_pagina);

return mmu;
}

t_tabla_de_paginas_cpu* buscarEnTLB(int num_pagina){
    
    if (tlb == NULL) {
        log_warning(logger, "La lista TLB no está inicializada.");
        return NULL;
    }

  if (strcmp(config_cpu->ALGORITMO_TLB, "LRU") == 0) {
        for (int i = 0; i < list_size(tlb); i++) {
            t_tabla_de_paginas_cpu* tlb_item = (t_tabla_de_paginas_cpu*) list_get(tlb, i);
            if (tlb_item == NULL) {
                continue;
            }
            tlb_item->contador++;
        }
    }

    for (int i = 0; i < list_size(tlb); i++){
        t_tabla_de_paginas_cpu* tabla_a_buscar = (t_tabla_de_paginas_cpu*)list_get(tlb,i); // recorremos la TLB
        //log_warning(logger, "Nro de pagina: %i", tabla_a_buscar->nropagina);
        if (tabla_a_buscar->nropagina == num_pagina){ //caundo hallamos la pagina requerida, returneamos
            if (strcmp(config_cpu->ALGORITMO_TLB, "LRU") == 0){
                t_tabla_de_paginas_cpu* tabla = (t_tabla_de_paginas_cpu*)list_remove(tlb,i);
                tabla->contador = 0;
                list_add_in_index(tlb,i,tabla);
            } 
            //log_warning(logger, "Nro de pagina: %i", tabla_a_buscar->nropagina);
            return tabla_a_buscar;

        }
    }
    return NULL;
}


t_tabla_de_paginas_cpu* actualizar_TLB_por_fifo(int numero_pagina){
        if (posicion_fifo < config_cpu->CANTIDAD_ENTRADAS_TLB){
            t_tabla_de_paginas_cpu* tabla   = list_remove(tlb, posicion_fifo); //Puede ser tlb 
            free(tabla);

            solicitar_tablas_a_memoria(numero_pagina); 
            t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
            if (nueva_tabla != NULL){
                nueva_tabla->pid = pcb->pid;
                nueva_tabla->nropagina = numero_pagina;
                log_info(logger,"PID %i -TLB MISS -Pagina %i",pcb->pid,nueva_tabla->nropagina);
                list_add_in_index(tlb,posicion_fifo,nueva_tabla);//Inserto en el lugar sacado
                ++posicion_fifo;
                return nueva_tabla;
            }else{
                log_error(logger, "No llego nada la tabla de memoria FIFO");
                return NULL;
                free(nueva_tabla);
            }
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
            ++posicion_fifo;
        }        
        
}


t_tabla_de_paginas_cpu* actualizar_Tlb_por_lru(int numero_pagina){
    int contador = -1;
    int indice = -1;


    for (int i = 0; i < list_size(tlb); i++){
        t_tabla_de_paginas_cpu* tabla = list_get(tlb,i);
        //log_warning(logger, "Posicion del lru: %i    || contador: %i", tabla->nropagina, tabla->contador);

        if (tabla->contador > contador){
            contador = tabla->contador;
            indice = i;
        }
    }
//log_warning(logger, "indice: %i", indice);
if (indice != -1){
   t_tabla_de_paginas_cpu* tabla_a_eliminar = (t_tabla_de_paginas_cpu*)list_remove(tlb,indice);
   log_nico(logger2, "Pagina extraida: %i", tabla_a_eliminar->nropagina);
    free(tabla_a_eliminar);

    solicitar_tablas_a_memoria(numero_pagina);
    t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
    //log_nico(logger2, "marco: %i <3", nueva_tabla->marco);
    if (nueva_tabla != NULL){
        nueva_tabla->contador = 0;
        nueva_tabla->nropagina = numero_pagina;
        nueva_tabla->pid = pcb->pid;  
        list_add(tlb, nueva_tabla); //Puede ser tlb
        log_info(logger,"PID %i -TLB MISS -Pagina %i",pcb->pid,nueva_tabla->nropagina);
        return nueva_tabla;
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