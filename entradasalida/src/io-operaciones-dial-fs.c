#include "io-operaciones-dial-fs.h"

void operacion_create_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos, t_list *archivos_abiertos) {

    // Obtenemos el path del archivo:
    char *name_file = list_get(argumentos, 0);

    if(existe_archivo_abierto(archivos_abiertos, name_file)) {
        log_error(logger, "El archivo %s ya se encuentra abierto", name_file);
        return;
    }

    t_config *archivo_metadata = iniciar_archivo_metadata(interfaz, name_file);

    // Buscamos un bloque libre:
    int bloque_libre = buscar_bloque_libre(bitmap, interfaz);

    if(bloque_libre == -1) {
        log_error(logger, "No hay bloques libres");
        return;
    }

    // Seteamos el bloque libre en el bitmap:
    setear_bloque_ocupado(bitmap, bloque_libre);

    // Seteamos el bloque libre en el archivo de metadata:
    set_bloque_libre_archivo_metadata(archivo_metadata, bloque_libre);

    // agregamos el archivo a la lista de archivos abiertos:
    agregar_a_archivos_abiertos(archivos_abiertos, archivo_metadata, name_file, bloque_libre);

    // Creamos el archivo:
    crear_archivo(archivo_metadata, interfaz, name_file);
     
}

void operacion_read_file(t_interfaz *interfaz, FILE *bloques, t_list *argumentos, t_list *archivos_abiertos) {

    // Obtenemos el archivo:
    char *name_file = list_get(argumentos, 0);
    t_archivo_abierto *archivo_abierto = get_archivo_abierto(archivos_abiertos, name_file);

    if(!archivo_abierto) {
        log_error(logger, "El archivo %s no se encuentra abierto", name_file);
        return;
    }

    t_config *archivo_metada = get_archivo_metadata(archivo_abierto);

    // Obtenemos los argumentos necesarios para la operación de lectura:
    int bloque_inicial;
    int tamanio_archivo;
    int bytes_a_leer;
    int offset;
    int dirreccion_fisica;

    // Procesamos la operación de lectura:
    procesar_operacio_de_lectura_o_escritura(archivo_metadata, argumentos, &bloque_inicial, &tamanio_archivo, &bytes_a_leer, &offset, &dirreccion_fisica);

    if(bytes_a_leer > tamanio_archivo) {
        log_error(logger, "No se pueden leer %d bytes de un archivo de %d bytes", bytes_a_leer, tamanio_archivo);
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
    char *name_file = list_get(argumentos, 0);
    t_archivo_abierto *archivo_abierto = get_archivo_abierto(archivos_abiertos, name_file);

    // Validamos que el archivo haya sido creado:
    if(!archivo_abierto) {
        log_error(logger, "El archivo %s no existe", name_file);

        return;
    }

    t_config *archivo_metada = get_archivo_metadata(archivo_abierto);

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


void operacion_delete_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos, t_list *archivos_abiertos) {

    // Obtenemos el archivo metadata:
    char *name_file = list_get(argumentos, 0);
    t_archivo_abierto *archivo_abierto = get_archivo_abierto(archivos_abiertos, name_file);

    t_config *archivo_metadata = get_archivo_metadata(archivo_abierto);

    // Liberamos los recursos del archivo:
    liberar_recuros_archivo(bitmap, archivo_metadata);

    // Eliminamos el archivo de la lista de archivos abiertos:
    eliminar_archivo_de_archivos_abiertos(archivos_abiertos, name_file);

    // Eliminamos el metadata del archivo:
    eliminar_metadata_archivo(archivo_metadata);
}

void operacion_truncate_file(t_interfaz *interfaz, FILE *bloques, t_bitarray *bitmap, t_list *argumentos, t_list *archivos_abiertos) {
    
    // Obtenemos el archivo metadata:
   char *name_file = list_get(argumentos, 0);
    t_archivo_abierto *archivo_abierto = get_archivo_abierto(archivos_abiertos, name_file);

    if(!archivo_abierto) {
        log_error(logger, "El archivo %s no existe", name_file);
        return;
    }

    t_config *archivo_metadata = get_archivo_metadata(archivo_abierto);

    int nuevo_tamanio = list_get(argumentos, 1);
    int tamanio_archivo = get_tamanio_archivo(archivo_metadata);

    // Vamos a aumentar el tamaño, entonces vamos asignar bloques:
    if (nuevo_tamanio > tamanio_archivo) { 
        int cantidad_de_bloques_nuevos_necesarios = calcular_cantidad_bloques_necesarios(interfaz, nuevo_tamanio);
        int tam_incrementado = nuevo_tamanio + tamanio_archivo; 

        // Compruebo si tengo bloques suficientes:
        if(hay_bloques_suficientes(bitmap, interfaz, cantidad_de_bloques_nuevos_necesarios)) {
            log_error(logger, "No hay bloques suficientes");
            return;
        }

        // Compruebo si los bloques necesarios están contiguos:
        if(!hay_bloques_contiguos(bitmap, interfaz, archivo_metada, cantidad_de_bloques_nuevos_necesarios)) {
            log_info(logger, "“PID: <%i> - Inicio Compactación.");
            compactar_archivos(bitmap, bloques, interfaz, archivos_abiertos, archivo_abierto, archivo_metada, cantidad_de_bloques_nuevos_necesarios);
            log_info(logger, "“PID: <%i> - Fin Compactación.");
        } else {
            asignar_bloques_nuevos(bitmap, interfaz, archivo_metada, cantidad_de_bloques_nuevos_necesarios);
            set_tamanio_archivo(archivo_metadata, tam_incrementado);
        }
    }
}