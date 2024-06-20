#include "memoria_peticiones.h"

sem_t enviar_instruccion;
sem_t sem_lectura_archivo;

void inicializar_semaforo() {
    sem_init(&enviar_instruccion, 0, 0);
    sem_init(&sem_lectura_archivo, 0, 1);
}
// void hacer_signal(){
//     sem_post(&enviar_instruccion);
// }

void* escuchar_peticiones(void* args){
    int socket_cliente = *(int*) args;
    inicializar_semaforo(); 

    while (1){   
        
        int cod_op = recibir_operacion(socket_cliente);
        //log_warning(logger,"cod op %i",cod_op);
        switch (cod_op){
        case HANDSHAKE:
            recibir_handshake(socket_cliente);
            break;
        case MENSAJE:
            recibir_mensaje(socket_cliente);
            //enviar_mensaje("MEMORIA -> CPU", socket_cliente);
            break;
        case HANDSHAKE_PAGINA:
            //log_warning(logger,"HANDSHAKE_PAGINA");
            recibir_handshake(socket_cliente);
            handshake_desde_memoria(socket_cliente);
            break;
        case INICIAR_PROCESO:
            // sem_wait(&sem_lectura_archivo); //semaforo para hacegurar que kernel nos mande un pseudo a la vez
            log_info(logger, "Iniciando proceso");
            retardo_pedido(config_memoria -> retardo_respuesta);
            leer_archivoPseudo(socket_cliente);
            // sem_post(&sem_lectura_archivo);
            enviar_respuesta_a_kernel(socket_cliente);
            break;
        case FINALIZAR_PROCESO:
            retardo_pedido(config_memoria -> retardo_respuesta);
            terminar_proceso(socket_cliente);
            break;
        case INSTRUCCION: 
            retardo_pedido(config_memoria -> retardo_respuesta);
            //sem_wait(&enviar_instruccion);
            log_info(logger,"Enviando instruccion a CPU");
            enviar_instruccion_a_cpu(socket_cliente);
            break;
        case ACCEDER_TABLA_PAGINAS: 
            retardo_pedido(config_memoria -> retardo_respuesta);
            obtener_marco(socket_cliente);
            break;
        case MODIFICAR_TAMAÑO_MEMORIA:
            retardo_pedido(config_memoria -> retardo_respuesta);
            resize_proceso(socket_cliente);
            break;
        case ACCESO_A_LECTURA:
            retardo_pedido(config_memoria -> retardo_respuesta);
            acceso_lectura(socket_cliente);
            break;
        case ACCESO_A_ESCRITURA:
            retardo_pedido(config_memoria -> retardo_respuesta);
            acceso_escritura(socket_cliente);
            break;
        case -1:
            log_info(logger,"Se desconecto el cliente");
            exit(-1); //a facu le gusta exit(-1) pero nico nos amenaza para poner return
        default:
            log_error(logger, "Operacion desconocida");
        }
    }
}

void handshake_desde_memoria(int socket_cliente) {
    t_paquete* paquete = crear_paquete(HANDSHAKE_PAGINA);
    int tam_pagina = config_memoria ->tam_pagina;

    // send(socket_cliente, &tam_pagina, sizeof(int), 0);
    
    agregar_a_paquete(paquete, &tam_pagina, sizeof(int));
    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void enviar_respuesta_a_kernel(int socket_cliente) {
    int respuesta = 1;

    send(socket_cliente, &respuesta, sizeof(int), 0);
}

//Creacion / destruccion de Tabla de Paginas: "PID:" <PID> "- Tamanio:" <CANTIDAD_PAGINAS>
void crear_proceso(int pid){
// void crear_proceso(int socket_cliente){
//     int size;
// 	void* buffer = recibir_buffer(&size, socket_cliente);
// 	int tamanio;
// 	int pid;
// 	memcpy(&pid, buffer, sizeof(int)); // creo que recibimos primero el pid, cambie el orden de los datos
// 	memcpy(&tamanio, buffer + sizeof(int), sizeof(int));

    //Creamos una nueva tabla de páginas
    t_list* tabla_de_paginas = list_create();

    // Agregar la tabla de páginas al diccionario
    char* pid_string=string_itoa(pid);

    dictionary_put(diccionario_paginas_porPID, pid_string, tabla_de_paginas);

    // Liberar el buffer recibido
    //free(buffer);  
}

void terminar_proceso(int socket_cliente){
    int size;
    void* buffer = recibir_buffer(&size, socket_cliente);
    int pid;
    memcpy(&pid, buffer, sizeof(int));

    char* pid_string = string_itoa(pid);

    // Obtener la tabla de páginas del diccionario
    t_list* tabla_de_paginas = dictionary_get(diccionario_paginas_porPID, pid_string);
    if (!tabla_de_paginas) {
        log_error(logger, "Error: No se encontró la tabla de páginas para el PID %d\n", pid);
        free(buffer);
        exit(-1);
    }

    int cantidad_de_paginas = list_size(tabla_de_paginas);//averiguamos cuantas paginas tiene
    for(int i = 0; i < cantidad_de_paginas; i++){ 
        log_warning(logger, "Se esta borrando la pagina %i", i);
        t_tabla_de_paginas* pag_a_eliminar = list_get(tabla_de_paginas, i); //obtengo la pagina [i]
        if(pag_a_eliminar == NULL){
            log_error(logger, "No se encontró la pagina %i", i);
            exit(-1);
        }
        bitarray_clean_bit(bitmap, pag_a_eliminar->marco); //marco en el bitmap la pagina [i] como "libre"
        free(pag_a_eliminar); //libero la pagina [i]
    }
    list_destroy(tabla_de_paginas); //destruyo la tabla de paginas
    log_info(logger,  "Destruccion -> PID: %d - Tamaño: %d", pid, cantidad_de_paginas); // Log minimo y obligatorio
    free(buffer);
}

//Ampliacion de Proceso: "PID:" <PID> "- Tamanio Actual:" <TAMANIO_ACTUAL> "- Tamanio a Ampliar:" <TAMANIO_A_AMPLIAR>
//Reduccion de Proceso: "PID:" <PID> "- Tamanio Actual:" <TAMANIO_ACTUAL> "- Tamanio a Reducir:" <TAMANIO_A_REDUCIR>
void resize_proceso(int socket_cliente) {

  	int size;
	void* buffer = recibir_buffer(&size, socket_cliente);
	int tamanio_bytes;
	int pid;
    memcpy(&pid, buffer, sizeof(int));
	memcpy(&tamanio_bytes, buffer + sizeof(int), sizeof(int));

    char* pid_string = string_itoa(pid);

    // Obtener la tabla de páginas del diccionario
    t_list* tabla_de_paginas = dictionary_get(diccionario_paginas_porPID, pid_string);
    if (!tabla_de_paginas) {
        log_error(logger, "Error: No se encontró la tabla de páginas para el PID %d\n", pid);
        t_paquete* paquete = crear_paquete(OUT_OF_MEMORY);
        int tabla_de_pag_no_encontrado = -1;
        agregar_a_paquete(paquete, &tabla_de_pag_no_encontrado , sizeof(int));
        enviar_paquete(paquete, socket_cliente);
        eliminar_paquete(paquete);
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

        t_paquete* paquete = crear_paquete(OUT_OF_MEMORY);
        int fuera_de_memoria = -1;
        agregar_a_paquete(paquete, &fuera_de_memoria , sizeof(int));
        enviar_paquete(paquete, socket_cliente);
        eliminar_paquete(paquete);
        free(buffer);
        
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    t_paquete* paquete = crear_paquete(EXITO_CONSULTA);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    int operacion_exitosa = 1;
    agregar_a_paquete(paquete, &operacion_exitosa, sizeof(int));
    eliminar_paquete(paquete);
    free(buffer);
}

//Acceso a Tabla de Paginas: "PID:" <PID> "- Pagina:" <PAGINA> "- Marco:" <MARCO>
void obtener_marco(int socket_cliente){

    int size;
	void* buffer = recibir_buffer(&size, socket_cliente);
    int pid;
	int pagina;
	memcpy(&pid, buffer, sizeof(int));
	memcpy(&pagina, buffer + sizeof(int), sizeof(int));

    // Pasamos el pid a string xq dictionary_get recibe si o si un string
    char* pid_string = string_itoa(pid);

    t_list* tabla_de_paginas = dictionary_get(diccionario_paginas_porPID , pid_string); //Buscamos la tabla de paginas del PID requerido
    if (!tabla_de_paginas) {
        log_error(logger, "Error: No se encontró la tabla de páginas para el PID %d\n", pid);
        exit(-1);
    }
    t_tabla_de_paginas* entrada = list_get(tabla_de_paginas, pagina);  //Obtenemos la pagina requerida
 
    if(entrada->bit_validez == 1){// si la pagina existe, se la enviamos a cpu
        t_paquete* marco_paquete = crear_paquete(ACCEDER_TABLA_PAGINAS);
        agregar_a_paquete(marco_paquete, &(entrada->marco), sizeof(int));
        enviar_paquete(marco_paquete, socket_cliente);
        log_info(logger, "PID: %c - Pagina: %d PAGINA> - %d <MARCO ,>" , pid, entrada->nro_pagina, entrada->marco);
    } 
    else{ //caso contrario enviamos el mensaje de error
        t_paquete* marco_paquete = crear_paquete(ACCEDER_TABLA_PAGINAS);
        int marco_no_encontrado = -1;
        agregar_a_paquete(marco_paquete, &marco_no_encontrado , sizeof(int));
        enviar_paquete(marco_paquete, socket_cliente);
        log_error(logger,"La pagina pedida no existe");
    }
    free(buffer);
}

int obtener_marco_libre(t_bitarray *bitmap){
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

void acceso_escritura(int socket_cliente){
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
        eliminar_paquete(paquete);
    }
    // se escribe en el espacio usuario el contenido a escribir
    memcpy(espacio_de_usuario + direc_fisica, contenido_a_escribir, tamanio_escritura);; 
    log_info(logger, "Acceso a espacio de usuario: PID: %d - Accion: ESCRIBIR - Direccion fisica: %d  - Tamaño: %d", pid, direc_fisica, tamanio_escritura);

    enviar_mensaje("OK", socket_cliente);

    free(contenido_a_escribir);
    free(buffer);
}