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
            generar_handshake(socket_cliente);
            break;
        case HANDSHAKE_PAGINA:
            handshake_desde_memoria(socket_cliente);
            break;
        case INICIAR_PROCESO: 
            retardo_pedido(config_memoria -> retardo_de_respuesta);
            crear_proceso(socket_cliente);
            leer_archivoPseudo(socket_cliente);
            break;
        case FINALIZAR_PROCESO:
            retardo_pedido(config_memoria -> retardo_de_respuesta);
            terminar_proceso(socket_cliente);
            break;
        case INSTRUCCION: 
            //El retardo ya esta oincluido en la funcion
            enviar_instruccion_a_cpu(socket_cliente, config_memoria -> retardo_de_respuesta);
            break;
        case ACCEDER_TABLA_PAGINAS: 
            retardo_pedido(config_memoria -> retardo_de_respuesta);
            obtener_marco(socket_cliente);
            break;
        case MODIFICAR_TAMAÑO_MEMORIA:
            retardo_pedido(config_memoria -> retardo_de_respuesta);
            resize_proceso(socket_cliente);
            break;
        case ACCESO_A_LECTURA:
            retardo_pedido(config_memoria -> retardo_de_respuesta);
            acceso_lectura(socket_cliente);
            break;
        case ACCESO_A_ESCRITURA:
            retardo_pedido(config_memoria -> retardo_de_respuesta);
            acceso_escritura(socket_cliente);
            break;
        default:
            log_error(logger, "Operacion desconocida");
            break;
        }
    }
}


//Funciones

void handshake_desde_memoria(int socket_cliente){
	int size;
	char* buffer = recibir_buffer(&size, socket_cliente);

	t_paquete* paquete = crear_paquete(HANDSHAKE_PAGINA);
	agregar_a_paquete(paquete, &tam_pagina, sizeof(tam_pagina));

	enviar_paquete(paquete, socket_cliente);

	eliminar_paquete(paquete);

	free(buffer);
}



//Creacion / destruccion de Tabla de Paginas: "PID:" <PID> "- Tamanio:" <CANTIDAD_PAGINAS>
void crear_proceso(socket_cliente){

  	int size;
	void* buffer = recibir_buffer(&size, socket_cliente);
	int tamanio;
	int pid;
	memcpy(&pid, buffer, sizeof(int)); // creo que recibimos primero el pid, cambie el orden de los datos
	memcpy(&tamanio, buffer + sizeof(int), sizeof(int));

    //Creamos una nueva tabla de páginas
    t_list* tabla_de_paginas = list_create();

    // Agregar la tabla de páginas al diccionario
    char* pid_string=string_itoa(pid)

    dictionary_put(diccionario_paginas_porPID, pid_string, tabla_de_paginas);

    // Liberar el buffer recibido
    free(buffer);
    
}

void terminar_proceso(socket_cliente){
    int size;
    void* buffer = recibir_buffer(&size, socket_cliente);
    int pid;
    memcpy(&pid, buffer, sizeof(int));

    char* pid_string = string_itoa(pid)

    // Obtener la tabla de páginas del diccionario
    t_tabla_pagina* tabla_de_paginas = dictionary_get(diccionario_paginas_porPID, pid_str);
    if (!tabla_de_paginas) {
        log_error(logger, "Error: No se encontró la tabla de páginas para el PID %d\n", pid);
        return -1;
    }

    int cantidad_de_paginas = list_size(tabla_de_paginas);//averiguamos cuantas paginas tiene
    for(int i=0; i < cantidad_de_paginas, i++){ 
        t_tabla_pagina* pag_a_eliminar = list_get(tabla_de_paginas, i); //obtengo la pagina [i]
        bitarray_clean_bit(bitmap, tabla_de_paginas->marco); //marco en el bitmap la pagina [i] como "libre"
        free(pag_a_eliminar); //libero la pagina [i]
    }
    list_destroy(tabla_de_paginas); //destruyo la tabla de paginas
    log_info(logger,  "PID: %d - Tamaño: %d", pid, cantidad_de_paginas); // Log minimo y obligatorio
    free(buffer);
}

//Ampliacion de Proceso: "PID:" <PID> "- Tamanio Actual:" <TAMANIO_ACTUAL> "- Tamanio a Ampliar:" <TAMANIO_A_AMPLIAR>
//Reduccion de Proceso: "PID:" <PID> "- Tamanio Actual:" <TAMANIO_ACTUAL> "- Tamanio a Reducir:" <TAMANIO_A_REDUCIR>
int resize_proceso(socket_cliente) {

  	int size;
	void* buffer = recibir_buffer(&size, socket_cliente);
	int tamanio_bytes;
	int pid;
    memcpy(&pid, buffer, sizeof(int));
	memcpy(&tamanio_bytes, buffer + sizeof(int), sizeof(int));

    char* pid_string = string_itoa(pid)

    // Obtener la tabla de páginas del diccionario
    t_tabla_pagina* tabla_de_paginas = dictionary_get(diccionario_paginas_porPID, pid_str);
    if (!tabla_de_paginas) {
        log_error(logger, "Error: No se encontró la tabla de páginas para el PID %d\n", pid);
        return -1;
    } // CHEQUEAR ESTO

    // Calcular el número de páginas necesarias
    int num_paginas = ceil((double)tamanio_bytes / 4); // La funcion ceil redondea hacia arriba

    int paginas_actuales = list_size(tabla_de_paginas); // Calculamos la cantidad de paginas que tiene la tabla
                                                        
    if (num_paginas > paginas_actuales) { // Si la cantidad de paginas de la tabla actual son menos al calculo del resize -> se amplia 
    // Ajustar el tamaño de la tabla de páginas
    log_info(logger, "PID: %d - Tamaño Actual: %d - Tamaño a Ampliar: %d ", pid, num_paginas*4 ,tamanio_bytes); // Log minimo y obligatorio
    for (int i = paginas_actuales; i < num_paginas; i++) {
        int marco_libre = obtener_marco_libre(bitmap);
        if (marco_libre == -1) {
            log_error(logger, "Out of memory (faltan marcos).\n");

            return -1; // CPU le avisa kernel que nos quedamos sin memoria 
        }

        //guardamos espacio de memoria para la nueva pagina
        t_tabla_de_paginas* nueva_pagina = malloc(sizeof(t_tabla_de_paginas));
        nueva_pagina->nro_pagina = i;
        nueva_pagina->marco = marco_libre;
        nueva_pagina->bit_validez = 1;

        list_add(tabla_de_paginas, nueva_pagina);
        }

    } else if (num_paginas < paginas_actuales) {
        // Reducir el tamaño del proceso
        log_info(logger, "PID: %d - Tamaño Actual: %d - Tamaño a Reducir: %d ", pid, num_paginas*4 ,tamanio_bytes); // Log minimo y obligatorio
        for (int i = paginas_actuales - 1; i >= num_paginas; i--) {
            t_tabla_de_paginas* pagina_a_eliminar = list_remove(tabla_de_paginas, i);
            liberar_marco(pagina_a_eliminar->marco); // Liberar el marco
            free(pagina_a_eliminar); // Liberar la memoria de la página
        }
    }
    free(buffer);
    return 1; // ta todo joya
}

//Acceso a Tabla de Paginas: "PID:" <PID> "- Pagina:" <PAGINA> "- Marco:" <MARCO>
void obtener_marco(socket_cliente){

    int size;
	void* buffer = recibir_buffer(&size, socket_cliente);
    int pid;
	int pagina;
    
	memcpy(&pid, buffer, sizeof(int));
	memcpy(&pagina, buffer + sizeof(int), sizeof(int));

    // Pasamos el pid a string xq dictionary_get recibe si o si un string
    char* pid_string = string_itoa(pid)

    t_tabla_pagina* tabla_de_paginas = dictionary_get(diccionario_paginas_porPID , pid_string); //Buscamos la tabla de paginas del PID requerido
    if (!tabla_de_paginas) {
        log_error(logger, "Error: No se encontró la tabla de páginas para el PID %d\n", pid);
        return -1;
    }
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

int obtener_marco_libre(t_bitarray *bitmap) {
    for (int i = 0; i < bitarray_get_max_bit(bitmap); i++) {
        if (!bitarray_test_bit(bitmap, i)) {  // Verifica si el bit está libre (es 0)
            bitarray_set_bit(bitmap, i); // Marca el bit como ocupado (pone 1)
            return i;
        }
    }
    return -1;  // No hay bits libres, OUT OF MEMORY?
}

void liberar_marco(int marco) {
    bitarray_clean_bit(bitmap, marco); // Marca el bit como libre (pone 0)
}

//Acceso a espacio de usuario: "PID:" <PID> "- Accion:" <LEER / ESCRIBIR> "- Direccion fisica:" <DIRECCION_FISICA> "- Tamanio" <TAMANIO A LEER / ESCRIBIR>
void acceso_lectura(int socket_cliente){
    int size;
    void* buffer = recibir_buffer(&size, socket_cliente);
    
    int desplazamiento = 0;
    int pid;
    int direc_fisica;
    int tamanio_lectura;

    memcpy(&pid, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&direc_fisica, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&tamanio_lectura, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    // Asignar memoria para el contenido a leer
    void* contenido_leer = malloc(tamanio_lectura);

    // Realizar la lectura del espacio de usuario
    memcpy(contenido_leer, espacio_de_usuario + direc_fisica, tamanio_lectura);
    log_info(logger, "Acceso a espacio de usuario: PID: %d - Accion: LEER - Direccion fisica: %d  - Tamaño: %d", pid, direc_fisica, tamanio_lectura);

    t_paquete* paquete = crear_paquete(LECTURA_EXITOSA);
    agregar_a_paquete(paquete, contenido_leer, tamanio_lectura);
    enviar_paquete(paquete, socket_cliente);

    eliminar_paquete(paquete);
    free(contenido_leer);
    free(buffer);
}

void acceso_escritura(socket_cliente){
    int size;
    void* buffer = recibir_buffer(&size, socket_cliente);
    
    int desplazamiento = 0;
    int pid;
    int direc_fisica;
    int tamanio_escritura;
    void* contenido_a_escribir;

    memcpy(&pid, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&direc_fisica, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);
    memcpy(&tamanio_escritura, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    // Asignar memoria para el contenido a escribir
    contenido_a_escribir = malloc(tamanio_escritura);
    // copiamos el contenido a escribir
    memcpy(contenido_a_escribir, buffer + desplazamiento, tamanio_escritura);

    // Verificar que no se escriba fuera del espacio de usuario
    if (direc_fisica + tamanio_escritura > config_memoria->tam_memoria) {
        log_error(logger, "Error: Se intentó escribir fuera del espacio de usuario\n");
        t_paquete* paquete = crear_paquete(OUT_OF_MEMORY);
        enviar_paquete(paquete, socket_cliente);
        return -1;
    }
    // se escribe en el espacio usuario el contenido a escribir
    memcpy(espacio_de_usuario + direc_fisica, contenido_a_escribir, tamanio_escritura);; 
    log_info(logger, "Acceso a espacio de usuario: PID: %d - Accion: ESCRIBIR - Direccion fisica: %d  - Tamaño: %d", pid, direc_fisica, tamanio_escritura);

    t_paquete* paquete = crear_paquete(LECTURA_EXITOSA);
    agregar_a_paquete(paquete, contenido_leer, tamanio_lectura);
    enviar_paquete(paquete, socket_cliente);

    eliminar_paquete(paquete);
    free(contenido_leer);
    free(buffer);
}