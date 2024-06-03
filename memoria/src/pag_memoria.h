#ifndef PAG_MEMORIA_H
#define PAG_MEMORIA_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/socket.h>
#include <utils/shared.h>
#include <utils/estructuras_compartidas.h>
#include <utils/protocolo.h>
#include <pthread.h>
#include <utils/logger.h>
#include <utils/instrucciones.h>

//Estructuras para paginacion simple

typedef struct 
{
    int nro_pagina;
    int marco;
    int bit_validez;
    // bool presencia;
    // bool modificado;
} t_tabla_de_paginas;

// typedef struct 
// {
//     int pid;
//     int nro_marco;
//     bool esLibre;
//  }t_marco;

// void inicializar_marcos(int cant_marcos);


#endif // PAG_MEMORIA_H

/*
buscador -> pid* -> Tabla*
                     -> pag -> marco -> bit_validez
                     -> pag2 -> marco -> bit_validez

dictonary_get (pid1,tabla->pag2)

dictonary_get (dictionarioDePaginas, pid1) -> tabla
Dictionario por pid  -> de paginas

 pid pag1

    pag2 
    pag3
    pag4

pid2 pag1
 pid2 pag2

pid2 pag3
pid2 pag4

pid3 pag1
pid3 pag2
pid3 pag3
pid3 pag4

 encontrar_marco_pedido(socket){
    pid=deserializar1
    pag=deserializar2

    t_tabla_pagina* tabla = diccionario_get(diccionario ,pid)
    int pagina_pedida=tabla->pag
    if(pagina_pedida->bit_validez=0)
        agregar
    else
        devolver_marco_pedido(socket kernell,pagina_pedida->marco)
/ / }


->tabla*->pag->marco
                        ->pag

-> Busca en la TLB
    -> Si no esta  en la TLB
    TLB MISS 

->  Buscar en la tabla de paginas(pid, nu mero_pag, cant _movimiento)
    -> P = 1 
        -> PAGE HIT
        -> Se agrega entrada a TLB

    ->  P = 0
    -> Busca marco libre (bitmap -> t_marco* marcos[CANT_MARCOS]) //CANT_MARCOS = TAM_MEMORIA/LONG_MARCO
        -> Si hay marco libre 
            -> Se marca como frame ocupado , y se marca la pagina como presente (P = 1)
            -> Se agrega  entrada a TLB

        -> Si no hay marco libre 
            -> Se elige un frame victima (por el algortimo de reemplazo)
            -> Se sustituye la pagina victima por la nueva pagina, la pagin a victima se marca como no presente (P = 0)
            -> Se marca el frame como ocupado y se marca la pagina como presente (P = 1)
            -> Se agrega entrada a TLB  
            
-> Se sigue con la ejecución de la instrucción