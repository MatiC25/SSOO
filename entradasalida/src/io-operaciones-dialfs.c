#include "io-operaciones-dialfs.h"

void operacion_create_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos, t_list *archivos_ya_abiertos) {

    // Obtenemos los argumentos:
    char *nombre_archivo = list_get(argumentos, 1);

    if(ya_esta_abierto(archivos_ya_abiertos, nombre_archivo)) {
        log_error(logger, "El archivo ya se encuentra abierto");

        return;
    }

    // Creamos el archivo:
    t_config *archivo_metadata = abrir_archivo_metadata_config(interfaz, nombre_archivo, "w");

    // Obtenemos el bloque inicial:
    int bloque_inicial = obtener_bloque_libre(bitmap, interfaz);

    if(bloque_inicial == -1) {
        log_error(logger, "No hay bloques libres para el archivo");

        return;
    }

    // Seteamos bloque inicial como ocupado:
    set_bloque_ocupado(bitmap, bloque_inicial);

    // Seteamos los datos del archivo:
    set_bloque_inicial_en_archivo_metadata(archivo_metadata, bloque_inicial);
    set_tamanio_archivo_en_archivo_metadata(archivo_metadata, 0);

    // Agregamos el archivo a la lista de archivos abiertos:
    set_nuevo_archivo_abierto(archivos_ya_abiertos, nombre_archivo, archivo_metadata);

    // Guardamos el archivo:
    set_archivo_metada_en_fs(archivo_metadata);

    // Logeamos operacion:
    log_info(logger, "Se creo el archivo %s", nombre_archivo);
    log_info(logger, "Bloque inicial: %d", bloque_inicial);
}

void operacion_write_file(t_interfaz *interfaz, FILE *bloques, t_list *argumentos, t_list *archivos_ya_abiertos) {

    // Obtenemos los argumentos:
    char *nombre_archivo = list_get(argumentos, 1);
    int offset = list_get(argumentos, 2);
    t_list *direcciones_fisicas = list_get(argumentos, 3);

    // Obtenemos el archivo abierto:
    t_archivo_abierto *archivo_abierto = obtener_archivo_abierto(archivos_ya_abiertos, nombre_archivo);

    if(!archivo_metadata) {
        log_error(logger, "El archivo no se encuentra abierto");

        return;
    }

    // Obtenemos el archivo metadata:
    t_config *archivo_metadata = get_archivo_metadata(archivo_abierto);

    // Obtenemos el bloque inicial:
    int bloque_inicial = get_bloque_inicial(archivo_metadata);

    // Obtenemos el contenido a escribir:
    char *contenido = rcv_contenido_a_mostrar(interfaz, direcciones_fisicas, *pid_proceso);

    // Escribimos el contenido en el archivo:
    fseek(bloques, bloque_inicial * get_tamanio_bloque(interfaz) + offset, SEEK_SET);
    fwrite(contenido, sizeof(char), strlen(contenido), bloques);
    fseek(bloques, 0, SEEK_SET);

    // Logeamos operacion:
    log_info(logger, "Se escribio en el archivo %s", nombre_archivo);
    log_info(logger, "Contenido: %s", contenido);
}

void operacion_read_file(t_interfaz *interfaz, FILE *bloques, t_list *argumentos, t_list *archivos_ya_abiertos) {
    
    // Obtenemos los argumentos:
    char *nombre_archivo = list_get(argumentos, 1);
    int offset = list_get(argumentos, 2);
    t_list *direcciones_fisicas = list_get(argumentos, 3);

    // Obtenemos el archivo abierto:
    t_archivo_abierto *archivo_abierto = obtener_archivo_abierto(archivos_ya_abiertos, nombre_archivo);

    if(!archivo_metadata) {
        log_error(logger, "El archivo no se encuentra abierto");

        return;
    }

    // Obtenemos el archivo metadata:
    t_config *archivo_metadata = get_archivo_metadata(archivo_abierto);

    // Obtenemos el bloque inicial:
    int bloque_inicial = get_bloque_inicial(archivo_metadata);

    // Obtenemos la cantidad de bytes a leer:
    int bytes_a_escribir = get_total_de_bytes(direcciones_fisicas);

    // Creamos el contenido:
    char *contenido = malloc(bytes_a_escribir);

    // Leemos el contenido del archivo:
    fseek(bloques, bloque_inicial * get_tamanio_bloque(interfaz) + offset, SEEK_SET);
    fread(contenido, sizeof(char), bytes_a_escribir, bloques);
    fseek(bloques, 0, SEEK_SET);

    // Enviamos el contenido a memoria:
    send_bytes_a_leer(interfaz, *pid_proceso, direcciones_fisicas, contenido, bytes_a_escribir);

    // Logeamos operacion:
    log_info(logger, "Se leyo el archivo %s", nombre_archivo);
    log_info(logger, "Contenido: %s", contenido);
}

void operacion_truncate_file(t_interfaz *interfaz, FILE *bloques, t_bitarray *bitmap, t_list *argumentos, t_list *archivos_abiertos) {
    
    // Obtenemos los argumentos:
    char *nombre_archivo = list_get(argumentos, 1);
    int nuevo_tamanio = list_get(argumentos, 2);

    // Obtenemos el archivo abierto:
    t_archivo_abierto *archivo_abierto = obtener_archivo_abierto(archivos_abiertos, nombre_archivo);

    if(!archivo_metadata) {
        log_error(logger, "El archivo no se encuentra abierto");

        return;
    }

    // Obtenemos el archivo metadata:
    t_config *archivo_metadata = get_archivo_metadata(archivo_abierto);
    
    // Obtenemos el tamanio actual del archivo:
    int tamanio_actual = get_tamanio_archivo(archivo_metadata);

    if(nuevo_tamanio < tamanio_actual){
        log_info(logger, "Se no se puede truncar el archivo a un tamanio menor al actual");

        return;
    }

    // Obtenemos bloques necesarios:
    int bloques_necesarios = obtener_bloques_necesarios(interfaz, nuevo_tamanio);

    // Para truncar el archivo necesitamos bloques libres suficientes:
    if(!hay_bloques_libres_suficientes(bitmap, interfaz, bloques_necesarios)) {
        log_error(logger, "No hay bloques libres suficientes para truncar el archivo");
        
        return;
    }

    // Si hay bloques libres suficientes, truncamos el archivo:
    if(!hay_bloques_contiguos_libres(interfaz, bitmap, bloque_inicial, nuevo_tamanio)) {
        log_info(logger, "Hay bloques libres suficientes para truncar el archivo");

        // Obtenemos tamanio nuevo y bloque final:
        int tam_resultante = nuevo_tamanio + tamanio_actual;
        int bloque_final = calcular_bloque_final(interfaz, bloque_inicial, tamanio_actual);

        // Seteamos el nuevo tamanio del archivo:
        set_tamanio_archivo_en_archivo_metadata(archivo_metadata, tam_resultante);

        // Seteamos los bloques como ocupados:
        set_bloques_como_ocupados(bitmap, bloque_final, bloques_necesarios);

        // Logeamos operacion:
        log_info(logger, "Se truncó el archivo %s", nombre_archivo);
        log_info(logger, "Nuevo tamanio: %d", tam_resultante);
        log_info(logger, "Bloques necesarios: %d", bloques_necesarios);
    }
}