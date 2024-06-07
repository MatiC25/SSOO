#include "mmu.h"

t_mmu_cpu* traducirDireccion(int direccionLogica){
    t_mmu_cpu* mmu = (t_mmu_cpu*)malloc(sizeof(t_mmu_cpu));
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