#include "pag_memoria.h"

//Funcion inicializar marcos
void inicializar_marcos(int cant_marcos){
    for(int i = 0; i < cant_marcos; i++){
            t_marco * marco = malloc(sizeof(t_marco));
            marco->pid = 0;
            marco->nro_marco = i;
            marco->esLibre = true;
            list_add(tabla_marcos, marco);
        }
}