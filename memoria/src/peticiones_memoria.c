#include "peticiones_memoria.h"

//Estructuras

void* escuchar_peticiones(void* args){
    t_procesar_server* args_hilo = (t_procesar_server*) args;
    char* server_name = args_hilo->server_name;
    int socket_cliente = args_hilo->socket_servidor;

    while (1)
    {
        op_code cod_op = recibir_operacion(socket_cliente);
        
        switch (cod_op)
        {
        case HANDSHAKE:
            saludo(socket_cliente);
            break;
        case PSEUDOCODIGO:
            leer_archivoPseudo(socket_cliente);
            break;
        case INSTRUCCION: 
            enviar_instruccion_a_cpu(socket_cliente, config_memoria->retardo_respuesta);
            break;
        case INICIAR_PROCESO: 
            crear_proceso(socket_cliente);
            break;
        case FINALIZAR_PROCESO:
            terminar_proceso(socket_cliente);
            break;
        case ACCEDER_TABLA_PAGINAS: 
            obtener_marco(socket_cliente);
            break;
        case MODIFICAR_TAMAÑO_MEMORIA:
            modificar_tamaño_memoria(socket_cliente, config_memoria->retardo_respuesta);
            break;
        case ACCESO_A_LECTURA:
            acceso_lectura(socket_cliente);
            break;
        case ACCESO_A_ESCRITURA:
            acceso_escrituratura(socket_cliente);
            break;
        default:
            log_error(logger, "Operacion desconocida");
            break;
        }
    }
}








//Funciones
//Creacion / destruccion de Tabla de Paginas: "PID:" <PID> "- Tamanio:" <CANTIDAD_PAGINAS>
void crear_proceso(socket_cliente){

  	int size;
	void* buffer = recibir_buffer(&size, socket_cliente);
	int tamanio;
	int pid;
	memcpy(&tamanio, buffer, sizeof(int));
	memcpy(&pid, buffer + sizeof(int), sizeof(int));

    //Creamos una nueva tabla de páginas
    t_list* tabla_de_paginas = list_create();

    // Agregar la tabla de páginas al diccionario
    // NO ESTOY SEGURO SI LO TENGO QUE AGREGAR COMO INT O COMO STRING, XQ LUEGO EN EL DICTIONARY_GET LO BUSCA SIENDO UN STRING
    dictionary_put(diccionario_paginas_porPID, &pid, tabla_de_paginas);

    // Liberar el buffer recibido
    free(buffer);
    
}


void terminar_proceso(socket_cliente){
    
}

//Acceso a Tabla de Paginas: "PID:" <PID> "- Pagina:" <PAGINA> "- Marco:" <MARCO>
void obtener_marco(socket_cliente);{

    int size;
	void* buffer = recibir_buffer(&size, socket_cliente);
    int pid;
	int pagina;
    
	memcpy(&pid, buffer, sizeof(int));
	memcpy(&pagina, buffer + sizeof(int), sizeof(int));

    // Pasamos el pid a string xq dictionary_get recibe si o si un string
    char* pid_string=string_itoa(pid)

    t_tabla_pagina* tabla_de_paginas = dictionary_get(diccionario_paginas_porPID , pid_string); //Buscamos la tabla de paginas del PID requerido
    t_tabla_pagina* entrada = list_get(tabla_de_paginas, pagina);  //Obtenemos la pagina requerida
 
    if(entrada->bit_validez == 1){ //fijarnos despues si vamos a usar bools o ints
        // si la pagina existe, se la enviamos a cpu
        t_paquete* marco_paquete = crear_paquete();
        agregar_a_paquete(marco_paquete, entrada->marco, sizedof(int))
        enviar_paquete(marco_paquete, socket_cliente);
        log_info(logger, "PID: %c - Pagina: %d PAGINA> - %d <MARCO ,>" , pid, entrada->nro_pagina, entrada->marco);
    } 
    else{ //caso contrario enviamos el mensaje 
        t_paquete* marco_paquete = crear_paquete();
        agregar_a_paquete(marco_paquete, (-1) , sizedof(int))
        enviar_paquete(marco_paquete, socket_cliente);
        log_error(logger,"La pagina pedida no existe");
    }
    free(buffer);
}

int obtener_marco_libre(){
    for(int i = 0; i < bitarray_get_max_bit(bitmap*); i++){
        if(bitarray_test_bit(bitmap, i)  == false){
            == true;
            return i;
        }
    }
    return -1;
}

agregar_marco(int pid, int pagina, int marco){
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


//Ampliacion de Proceso: "PID:" <PID> "- Tamanio Actual:" <TAMANIO_ACTUAL> "- Tamanio a Ampliar:" <TAMANIO_A_AMPLIAR>
//Reduccion de Proceso: "PID:" <PID> "- Tamanio Actual:" <TAMANIO_ACTUAL> "- Tamanio a Reducir:" <TAMANIO_A_REDUCIR>
void modificar_tamaño_memoria(socket_cliente, config_memoria->retardo_respuesta);{

}


//Acceso a espacio de usuario: "PID:" <PID> "- Accion:" <LEER / ESCRIBIR> "- Direccion fisica:" <DIRECCION_FISICA> "- Tamanio" <TAMANIO A LEER / ESCRIBIR>
void acceso_lectura(socket_cliente);{

}

void acceso_escritura(socket_cliente);{
    
}

/*
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
            -> se devuelve error al que lo pida
            
-> Se sigue con la ejecución de la instrucción

  
        int posicion_libre = obtener_marco_libre(bitmap, cant_marcos);
        if (posicion_libre != -1){ //rebisamos si hay espacio para agregar la pagina
            agregar_marco(pid, pagina, posicion_libre); //agregamos la pagina al marco
            t_paquete* marco_paquete = crear_paquete();
            agregar_a_paquete(marco_paquete, entrada->marco, sizedof(int))
            enviar_paquete(marco_paquete, socket_cliente);
        }
        else{     //de no haberlo, se devolvemos OUT_OF_MEMORY a cpu y termianmos la instruccion
            t_paquete* paquete = crear_paquete(OUT_OF_MEMORY);
            agregar_a_paquete(error_paquete, "No hay marcos libres", sizeof("No hay marcos libres"));
            enviar_paquete(error_paquete, socket_cliente);