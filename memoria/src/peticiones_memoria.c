#include "peticiones_memoria.h"

//Estructuras

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
    t_list* tabla = list_create();

    // Agregar la tabla de páginas al diccionario
    // NO ESTOY SEGURO SI LO TENGO QUE AGREGAR COMO INT O COMO STRING, XQ LUEGO EN EL DICTIONARY_GET LO BUSCA SIENDO UN STRING
    dictionary_put(diccionario_paginas_porPID, &pid, tabla);

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

    t_tabla_pagina* tabla = dictionary_get(diccionario_paginas_porPID , pid_string); //Buscamos la tabla de paginas del PID requerido
    t_tabla_pagina* entrada = list_get(tabla, pagina);  //Obtenemos la pagina requerida
 
    if(entrada->bit_validez == 1){ //fijarnos despues si vamos a usar bools o ints
        // si la pagin a esta en los marcos, se la enviamos a cpu
        t_paquete* marco_paquete = crear_paquete();
        agregar_a_paquete(marco_paquete, entrada->marco, sizedof(int))
        enviar_paquete(marco_paquete, socket_cliente);
    } //caso contrario hay que agregarla y enviar el mensaje
    else{
        agregar_marco();///////////////////////////////////////FALTA////////////////////////////////////////////
        t_paquete* marco_paquete = crear_paquete();
        agregar_a_paquete(marco_paquete, entrada->marco, sizedof(int))
        enviar_paquete(marco_paquete, socket_cliente);
    }
}


//Ampliacion de Proceso: "PID:" <PID> "- Tamanio Actual:" <TAMANIO_ACTUAL> "- Tamanio a Ampliar:" <TAMANIO_A_AMPLIAR>
void ampliar_memoria(socket_cliente, config_memoria->retardo_respuesta);{

}

//Reduccion de Proceso: "PID:" <PID> "- Tamanio Actual:" <TAMANIO_ACTUAL> "- Tamanio a Reducir:" <TAMANIO_A_REDUCIR>
void reducir_memoria(socket_cliente, config_memoria->retardo_respuesta);{

}


//Acceso a espacio de usuario: "PID:" <PID> "- Accion:" <LEER / ESCRIBIR> "- Direccion fisica:" <DIRECCION_FISICA> "- Tamanio" <TAMANIO A LEER / ESCRIBIR>
void acceso_lectura(socket_cliente);{

}

void acceso_escritura(socket_cliente);{
    
}










