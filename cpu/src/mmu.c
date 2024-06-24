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
   log_info(logger, "PAgina: %i", pagina);
    int *pagina_ptr = malloc(sizeof(int));
    *pagina_ptr = pagina;
    list_add(mmu->num_pagina , pagina_ptr); //Primera posicion
     //log_nico(logger2, "Se agregó una pagina a la mmu");
     //log_nico(logger2, "Se la tabla de la mmu de paginas es de: %i", list_size(mmu->num_pagina));
     //log_info(logger, "DIreccion logica: %i", direccionLogica);

    mmu->ofset = direccionLogica - pagina * config_cpu->TAMANIO_PAGINA ;
    //log_info(logger,"Ofset: %i",mmu->ofset);

    //int tamanio_Actualizado = (int)floor((double)tamanio/config_cpu->TAMANIO_PAGINA); 
    
    int tamanio_Actualizado = config_cpu->TAMANIO_PAGINA - mmu->ofset;
    if (tamanio_Actualizado < tamanio){
    int* tamanio_ptr = malloc(sizeof(int));
    *tamanio_ptr = tamanio_Actualizado; 
    list_add(mmu->tamanio, tamanio_ptr);
    }else{
        int* ptro = malloc(sizeof(int));
        *ptro = tamanio;
        list_add(mmu->tamanio, ptro);
        //log_info(logger,"TAMANIO: %i",*ptro);
    }
    

    int direccionLogica_actualizada = (pagina + 1) * config_cpu->CANTIDAD_ENTRADAS_TLB;

    tamanio_Actualizado = tamanio - tamanio_Actualizado;
    //log_warning(logger, "TAMANIO ACTUALIZADO: %i", tamanio_Actualizado);

    while (config_cpu ->TAMANIO_PAGINA <= tamanio_Actualizado){ 
        int pagina_actualizada = (int)floor((double)direccionLogica_actualizada / config_cpu->TAMANIO_PAGINA);
        log_info(logger, "Nro pagina actualizada: %i", pagina_actualizada);
        int* pagina_ptr_nuevo = malloc(sizeof(int));
        *pagina_ptr_nuevo = pagina_actualizada; //Posicion N
        list_add(mmu->num_pagina , pagina_ptr_nuevo);

        direccionLogica_actualizada = pagina_actualizada * config_cpu->CANTIDAD_ENTRADAS_TLB;
        tamanio_Actualizado = tamanio_Actualizado  - config_cpu -> TAMANIO_PAGINA;

        int* tamanio_ptr_nuevo = malloc(sizeof(int));
        *tamanio_ptr_nuevo = tamanio_Actualizado;
        list_add(mmu->tamanio, tamanio_ptr_nuevo);
    }

    if (tamanio_Actualizado  > 0){
        //Ultima posicion
         int ultima_pagina = (int)floor((double)direccionLogica_actualizada / config_cpu->TAMANIO_PAGINA);
         log_info(logger,"ENTRO A ULTIMA PAGINA");
        int* pagina_ptr_ultima = malloc(sizeof(int));
        *pagina_ptr_ultima = ultima_pagina;
        list_add(mmu->num_pagina , pagina_ptr_ultima);

        int* tamanio_ptr_ultimo = malloc(sizeof(int));
        *tamanio_ptr_ultimo = tamanio_Actualizado;
        list_add(mmu->tamanio, tamanio_ptr_ultimo);
    }


for (int i = 0; i < list_size(mmu->num_pagina); i++) {
    int* pagina = (int*)list_get(mmu->num_pagina, i);
    int pagina_a_buscar = *pagina;
    //log_warning(logger, "PAGINA A BUSACAR: %i", pagina_a_buscar);

    t_tabla_de_paginas_cpu* tabla = (t_tabla_de_paginas_cpu*)buscarEnTLB(pagina_a_buscar);
        
    if (tabla == NULL) { // NO TLB
        //log_info(logger, "DIO NULL??");
        //log_info(logger, "Tamano TLB: %i", list_size(tlb));
        
        if (list_size(tlb) < config_cpu->CANTIDAD_ENTRADAS_TLB) {
            
            solicitar_tablas_a_memoria(pagina_a_buscar);
            t_tabla_de_paginas_cpu* tab = recv_tablas();
            tab->pid = pcb->pid;
            tab->contador = 0;
            tab->nropagina = pagina_a_buscar;
            
            //log_info(logger, "(EN TRADUCCION)pid: %i", tab->pid);
            //log_info(logger, "(EN TRADUCCION)marco: %i", tab->marco);
            //log_info(logger, "(EN TRADUCCION)nro pagina: %i", tab->nropagina);

            list_add(tlb, tab);

            int dirc_fisica = tab->marco * config_cpu->TAMANIO_PAGINA + mmu->ofset;
            //log_info(logger, "Direccion fisica calculada: %i", dirc_fisica);

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
                //log_info(logger, "Entro a FIFO");
                t_tabla_de_paginas_cpu* tab = (t_tabla_de_paginas_cpu*)actualizar_TLB_por_fifo(pagina_a_buscar);
                log_warning(logger, "CANTIDAD DE TLB: %i", list_size(tlb));
                int dirc_fisica = tab->marco * config_cpu->TAMANIO_PAGINA + mmu->ofset;
                //log_info(logger, "Direccion fisica calculada: %i", dirc_fisica);

                int* ptr_dirc_fisica = malloc(sizeof(int));
                if (ptr_dirc_fisica == NULL) {
                    log_error(logger, "Error al asignar memoria para ptr_dirc_fisica");
                    return NULL;
                }

                *ptr_dirc_fisica = dirc_fisica;
                list_add(mmu->direccionFIsica, ptr_dirc_fisica);

            } else if (strcmp(config_cpu->ALGORITMO_TLB, "LRU") == 0) {
                //log_info(logger, "Entro a LRU");
                t_tabla_de_paginas_cpu* tab = (t_tabla_de_paginas_cpu*)actualizar_Tlb_por_lru(pagina_a_buscar);      
                log_nico(logger2, "CANTIDAD DE TLB DESPUES DE LRU: %i", list_size(tlb));

                int dirc_fisica = tab->marco * config_cpu->TAMANIO_PAGINA + mmu->ofset;
                //log_info(logger, "Direccion fisica calculada: %i", dirc_fisica);

                int* ptr_dirc_fisica = malloc(sizeof(int));
                if (ptr_dirc_fisica == NULL) {
                    log_error(logger, "Error al asignar memoria para ptr_dirc_fisica");
                    return NULL;
                }

                *ptr_dirc_fisica = dirc_fisica;
                list_add(mmu->direccionFIsica, ptr_dirc_fisica);
                
            } else {
                log_error(logger, "Algoritmo no es valido en mmu");
                free(mmu);
                return NULL;
            }
        }
    } else {
        int dirc_fisica = tabla->marco * config_cpu->TAMANIO_PAGINA + mmu->ofset;
        //log_info(logger, "DF: %i", dirc_fisica);
        
        int* ptr_dirc_fisica = malloc(sizeof(int));
        if (ptr_dirc_fisica == NULL) {
            log_error(logger, "Error al asignar memoria para ptr_dirc_fisica");
            return NULL;
        }

        *ptr_dirc_fisica = dirc_fisica;

        log_info(logger, "PID %i - TLB HIT - Pagina: %i", pcb->pid, tabla->nropagina);
        log_info(logger, "PID %i - OBTENER MARCO - Pagina: %i - Marco: %i", pcb->pid, tabla->nropagina, tabla->marco);
        
        list_add(mmu->direccionFIsica, ptr_dirc_fisica);
    }
    //free(tabla);
}
free(mmu->num_pagina);

return mmu;
}

t_tabla_de_paginas_cpu* buscarEnTLB(int num_pagina){
    
    if (tlb == NULL) {
        log_warning(logger, "La lista TLB no está inicializada.");
        return NULL;
    }

 //log_warning(logger, "CANTIDAD DE TLB: %i", list_size(tlb));
  if (strcmp(config_cpu->ALGORITMO_TLB, "LRU") == 0) {
        for (int i = 0; i < list_size(tlb); i++) {
            t_tabla_de_paginas_cpu* tlb_item = (t_tabla_de_paginas_cpu*) list_get(tlb, i);
            if (tlb_item == NULL) {
                //log_warning(logger, "Elemento NULL encontrado en la TLB en la posición %d", i);
                continue;
            }
            tlb_item->contador++;
            //log_info(logger, "Contado: %i", tlb_item->contador);
            // free(tlb_item); // No liberes aquí si vas a seguir usándolo
        }
    }

    for (int i = 0; i < list_size(tlb); i++){
        t_tabla_de_paginas_cpu* tabla_a_buscar = (t_tabla_de_paginas_cpu*)list_get(tlb,i); // recorremos la TLB
        if (tabla_a_buscar->nropagina == num_pagina){ //caundo hallamos la pagina requerida, returneamos
            if (strcmp(config_cpu->ALGORITMO_TLB, "LRU") == 0){
                
                t_tabla_de_paginas_cpu* tabla = (t_tabla_de_paginas_cpu*)list_remove(tlb,i);
                //log_info(logger, "tabla_a_buscar->contador: %i", tabla->contador);
                tabla->contador = 0;
                //log_info(logger, "tabla_a_buscar->contador: %i", tabla->contador);
                list_add_in_index(tlb,i,tabla);
            } 
            // log_info(logger, "      marco: %i", tabla_a_buscar->marco);
            // log_info(logger, "      Nro Pagina: %i", tabla_a_buscar->nropagina);
            // log_info(logger, "      Pid: %i", tabla_a_buscar->pid);

            return tabla_a_buscar;
            //free(tlb);
        }
    }
    return NULL;
}


t_tabla_de_paginas_cpu* actualizar_TLB_por_fifo(int numero_pagina){
//Segunda opcion pero aca no mantengo la tabla  
    // t_tabla_de_paginas_cpu* victima = list_remove(tlb, 0);
    // free(victima);

    // solicitar_tablas_a_memoria(numero_pagina);
    // t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
    // if (nueva_tabla != NULL){
    //     list_add(tlb,nueva_tabla);
    // }
        
        if (posicion_fifo <= config_cpu->CANTIDAD_ENTRADAS_TLB){
            t_tabla_de_paginas_cpu* tabla   = list_remove(tlb, posicion_fifo); //Puede ser tlb 
            log_nico(logger2, "TABLA SACADA: %i", tabla->nropagina);
            free(tabla);
           // log_warning(logger, "CANTIDAD DE TLB: %i", list_size(tlb));
            solicitar_tablas_a_memoria(numero_pagina); 
            t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
            if (nueva_tabla != NULL){
                nueva_tabla->pid = pcb->pid;
                nueva_tabla->nropagina = numero_pagina;
                log_info(logger,"PID %i -TLB MISS -Pagina %i",pcb->pid,nueva_tabla->nropagina);
                list_add_in_index(tlb,posicion_fifo,nueva_tabla);//Inserto en el lugar sacado
                posicion_fifo++;
                return nueva_tabla;
                //free(nueva_tabla);
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
            posicion_fifo++;
        }        
        
}


t_tabla_de_paginas_cpu* actualizar_Tlb_por_lru(int numero_pagina){
    int contador = -1;
    int indice = -1;

    //log_warning(logger, "CANTIDAD DE TLB: %i", list_size(tlb));
    for (int i = 0; i < list_size(tlb); i++){
        t_tabla_de_paginas_cpu* tabla = list_get(tlb,i);
        //log_info(logger,"Numero pag: %i", tabla->nropagina);
        if (tabla->contador > contador){
            contador = tabla->contador;
            //log_info(logger,"contador: %i", contador);
            indice = i; 

        }
    }
    //log_info(logger, "inidice:%i", indice);
if (indice != -1)
{
    //log_warning(logger, "CANTIDAD DE TLB: %i", list_size(tlb));
   t_tabla_de_paginas_cpu* tabla_a_eliminar = list_remove(tlb,indice);
   log_nico(logger2, "TABLA SACADA: %i", tabla_a_eliminar->nropagina);
    free(tabla_a_eliminar);
    //log_warning(logger, "CANTIDAD DE TLB: %i", list_size(tlb));

    solicitar_tablas_a_memoria(numero_pagina);
    t_tabla_de_paginas_cpu* nueva_tabla = recv_tablas();
    if (nueva_tabla != NULL){
        log_info(logger,"PID %i -TLB MISS -Pagina %i",pcb->pid,nueva_tabla->nropagina);
        nueva_tabla->contador = 0;
        nueva_tabla->nropagina = numero_pagina;
        nueva_tabla->pid = pcb->pid;  
        list_add(tlb, nueva_tabla); //Puede ser tlb
        return nueva_tabla;
        //free(nueva_tabla);
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

