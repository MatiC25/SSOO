#include "io-operaciones-dialfs.h"

void operacion_create_file(t_interfaz *interfaz, t_bitarray *bitmap, t_list *argumentos, t_list *archivos_ya_abiertos) {

    // Obtenemos los argumentos:
    char *nombre_archivo = list_get(argumentos, 0);

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
    set_bloque_ocupado(bitmap, interfaz, bloque_inicial);

    // Seteamos los datos del archivo:
    set_bloque_inicial_en_archivo_metadata(archivo_metadata, bloque_inicial);
    set_tamanio_archivo_en_archivo_metadata(archivo_metadata, 0);

    // Agregamos el archivo a la lista de archivos abiertos:
    set_nuevo_archivo_abierto(archivos_ya_abiertos, nombre_archivo, archivo_metadata);

    // Guardamos el archivo:
    set_archivo_metada_en_fs(archivo_metadata);

    // Logeamos operacion:
    log_info(logger, "Archivo: %s", nombre_archivo);
    log_info(logger, "Bloque inicial: %i", bloque_inicial);
}