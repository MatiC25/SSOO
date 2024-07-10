#include "memoria_globales.h"

t_dictionary * diccionario_paginas_porPID;
t_dictionary* lista_instrucciones_porPID;
void* espacio_de_usuario;
t_config_memoria* config_memoria;
void* memoria_usuario_bitmap;
t_bitarray* bitmap;


void crear_espacio_usuario() {
    espacio_de_usuario = malloc (config_memoria->tam_memoria); // reservamos memoria para el espacio de usuario
}

void crear_bitmap(){
    int cantidad_marcos = config_memoria->tam_memoria / config_memoria->tam_pagina; //calculamos la cantidad de marcos
    memoria_usuario_bitmap = malloc(cantidad_marcos/8);// reservamos memoria para el bitmap
    bitmap = bitarray_create_with_mode(memoria_usuario_bitmap , cantidad_marcos/8, LSB_FIRST);
}

void crear_config_memoria(){
    config_memoria = inicializar_config_memoria();
}

void crear_diccionario_paginas_porPID(){
    diccionario_paginas_porPID = dictionary_create();
}

void inicializacion_diccionario() {
    lista_instrucciones_porPID = dictionary_create();
    if (lista_instrucciones_porPID == NULL) {
        log_error(logger, "Error al crear el diccionario");
        exit(1);
    }
}

void cerrar_programa_memoria(int signal) {
    if(signal == SIGINT) {
        log_info(logger, "Cerrando programa memoria");

        //liberamos la memoria que reservamos
        dictionary_destroy_and_destroy_elements(lista_instrucciones_porPID, liberar_lista_instrucciones);
        dictionary_destroy_and_destroy_elements(diccionario_paginas_porPID, liberar_paginas_porPID);

        free(espacio_de_usuario);
        config_destroy_version_memoria(config_memoria);
        bitarray_destroy(bitmap);
        free(memoria_usuario_bitmap);
        exit(0);
    }
}

void liberar_lista_instrucciones(void* lista){
    t_list *lista_instrucciones = lista;
    list_destroy_and_destroy_elements(lista_instrucciones, elemento_lista_instrucciones_destroyer);
}

void elemento_lista_instrucciones_destroyer(void* elemento) {
    t_list *parametros = elemento;
    list_destroy_and_destroy_elements(parametros, free);
}

void liberar_paginas_porPID(void *paginas) {
    t_list *lista_paginas = paginas;
    list_destroy_and_destroy_elements(lista_paginas, free);
}

void configurar_senial_cierre() {
    struct sigaction sa;
    sa.sa_handler = cerrar_programa_memoria;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("Error al configurar la senial de cierre");
        exit(1);
    }
}