#include "io-operaciones-dial-fs.h"

void operacion_create_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos) {

    // Obtenemos el path del archivo:
    char *name_file = list_get(argumentos, 0);
    t_config *archivo_metadata = iniciar_archivo_metadata(interfaz, name_file);

    // Buscamos un bloque libre:
    int bloque_libre = buscar_bloque_libre(bitmap);

    if(bloque_libre == -1) {
        logger_error(logger, "No hay bloques libres");
        return;
    }

    // Seteamos el bloque libre en el bitmap:
    setear_bloque_ocupado(bitmap, bloque_libre);

    // Seteamos el bloque libre en el archivo de metadata:
    set_bloque_libre_archivo_metadata(archivo_metadata, bloque_libre);
}

void operacion_read_file(t_interfaz *interfaz, FILE *bloques, t_list *argumentos) {

    // Obtenemos el archivo metadata:
    t_config *archivo_metadata = get_archivo_config_from_args(interfaz, argumentos);

    // Validamos que el archivo haya sido creado:
    if(!es_un_archivo_valido(archivo_metadata)) {
        logger_error(logger, "El archivo %s no existe", name_file);
        return;
    }

    // Obtenemos los argumentos necesarios para la operación de lectura:
    int bloque_inicial;
    int tamanio_archivo;
    int bytes_a_leer;
    int offset;
    int dirreccion_fisica;

    // Procesamos la operación de lectura:
    procesar_operacio_de_lectura_o_escritura(archivo_metadata, argumentos, &bloque_inicial, &tamanio_archivo, &bytes_a_leer, &offset, &dirreccion_fisica);

    if(bytes_a_leer > tamanio_archivo) {
        logger_error(logger, "No se pueden leer %d bytes de un archivo de %d bytes", bytes_a_leer, tamanio_archivo);
        return;
    }

    // Creamos un buffer para almacenar el contenido:
    unsigned char* contenido = malloc(bytes_a_leer);

    // Leemos el contenido de los bloques:
    leer_contenido_desde_bloques(bloques, bloque_inicial, offset, bytes_a_leer, contenido, interfaz);

    // Falta implementar como enviamos a memoria:
}

void operacion_write_file(t_interfaz *interfaz, FILE *bloques, t_list *argumentos) {

    // Obtenemos el archivo metadata:
    t_config *archivo_metadata = get_archivo_config_from_args(interfaz, argumentos);

    // Validamos que el archivo haya sido creado:
    if(!es_un_archivo_valido(archivo_metadata)) {
        logger_error(logger, "El archivo %s no existe", name_file);
        return;
    }

    // Obtenemos los argumentos necesarios para la operación de lectura:
    int bloque_inicial;
    int tamanio_archivo;
    int bytes_a_escribir;
    int offset;
    int dirreccion_fisica;

    // Procesamos la operación de escritura:
    procesar_operacio_de_lectura_o_escritura(archivo_metadata, argumentos, &bloque_inicial, &tamanio_archivo, &bytes_a_escribir, &offset, &dirreccion_fisica);

    unsigned char *contenido; // Contenido a escribir

    // Enviamos los bytes a leer a memoria y recibimos el contenido a escribir:
    send_bytes_a_leer(interfaz, dirreccion_fisica, bytes_a_escribir);
    rcv_contenido_a_escribir(interfaz, bytes_a_escribir, &contenido);
    
    // Escribimos el contenido en los bloques:
    escribir_contenido_en_bloques(bloques, bloque_inicial, offset, tamanio_archivo, contenido, bytes_a_escribir, interfaz);
}


void operacion_delete_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos) {

    // Obtenemos el archivo metadata:
    char *name_file = list_get(argumentos, 0);
    t_config *archivo_metadata = iniciar_archivo_config(interfaz, name_file);

    // Validamos que el archivo haya sido creado:
    if(!es_un_archivo_valido(archivo_metadata)) {
        logger_error(logger, "El archivo %s no existe", name_file);
        return;
    }

    // Obtenemos el bloque inicial y el tamaño del archivo:
    int bloque_inicial = get_bloque_inicial(archivo_metadata);
    int tamanio_archivo = get_tamanio_archivo(archivo_metadata);

    // Liberamos el bloque en el bitmap:
    liberar_bloque(bitmap, bloque_inicial);
}

