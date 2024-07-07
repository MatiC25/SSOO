#include "io-compactacion.h"

void compactar_fs(t_interfaz  *interfaz, FILE *bloques, t_bitarray *bitmap, t_list *archivos_ya_abiertos, t_config *archivo_metadata, int cantidad_bloques_asignados_a_archivo_compactar, int bloque_inicial_archivo_a_compactar, int tam_nuevo_archivo_a_compactar) {

    // Obtenemos la cantidad de archivos abiertos:
    int cantidad_archivos_abiertos = list_size(archivos_ya_abiertos);

    // Ordenamos la lista de archivos abiertos por bloque inicial:
    t_list *archivos_ordenados = list_sorted(archivos_ya_abiertos, (void *) comparar_bloque_inicial);

    // Creamos un buffer para los archivos:
    t_queue *buffer = queue_create();

    // Inicializamos variables:
    int bloque_libre;
    int bloque_inicial_archivo_actual;
    int tamanio_archivo_archivo_actual;
    int cantidad_bloques_asignados;

    for(int i =  0; i < cantidad_archivos_abiertos; i++) {
        t_archivo_abierto *abierto_actual_abierto = list_get(archivos_ordenados, i);

        // Obtenemos el archivo actual:
        t_config *archivo_metadata_archivo_actual = get_archivo_metadata(abierto_actual_abierto);

        // Obtenemos el bloque inicial y el tamanio del archivo actual:
        bloque_inicial_archivo_actual = get_bloque_inicial(archivo_metadata_archivo_actual);
        tamanio_archivo_archivo_actual = get_tamanio_archivo(archivo_metadata_archivo_actual);

        // Obtenemos un bloque libre:
        bloque_libre = obtener_bloque_libre(bitmap, interfaz);

        if(bloque_libre > bloque_inicial_archivo_actual || bloque_inicial_archivo_actual == bloque_inicial_archivo_a_compactar) {
            cantidad_bloques_asignados = calcular_cantidad_bloques_asignados(interfaz, tamanio_archivo_archivo_actual);

            // Liberamos los bloques asignados:
            liberar_bloques_asignados(bitmap, cantidad_bloques_asignados, bloque_inicial_archivo_actual);

            // Creamos un buffer para el archivo actual:
            unsigned char *buffer = malloc(tamanio_archivo_archivo_actual);

            // Obtenemos los datos del archivo actual:
            fseek(bloques, bloque_inicial_archivo_actual * get_block_size(interfaz), SEEK_SET);
            fread(buffer, tamanio_archivo_archivo_actual, 1, bloques);
            fseek(bloques, 0, SEEK_SET);

            if(bloque_inicial_archivo_actual != bloque_inicial_archivo_a_compactar) {
                // Seteamos los bloques como ocupados:
                set_bloques_como_ocupados_desde(bitmap, bloque_libre, cantidad_bloques_asignados);

                // Seteamos los datos del archivo actual:
                set_bloque_inicial_en_archivo_metadata(archivo_metadata, bloque_libre);

                // Guardamos el archivo:
                set_archivo_metada_en_fs(archivo_metadata);

                // Guardamos los datos del archivo actual:
                queue_push(buffer, buffer);
            } else {
                // Guardamos los datos del archivo a compactar:
                unsigned char *buffer_archivo_a_compactar = malloc(tamanio_archivo_archivo_actual);

                // Obtenemos los datos del archivo a compactar:
                buffer_archivo_a_compactar = buffer;
            }
        }
    }

    // Guardamos los datos del archivo a compactar:
    queue_push(buffer, buffer_archivo_a_compactar);

    // Obtenemos el un bloque libre:
    bloque_libre = calcular_bloque_final(interfaz, bloque_libre, tamanio_archivo_archivo_actual) + 1;
    cantidad_bloques_asignados = calcular_cantidad_bloques_asignados(interfaz, tam_nuevo_archivo_a_compactar) + cantidad_bloques_asignados_a_archivo_compactar;

    // Seteamos los bloques como ocupados:
    set_bloques_como_ocupados_desde(bitmap, bloque_libre, cantidad_bloques_asignados);

    // Seteamos los datos del archivo a compactar:
    set_bloque_inicial_en_archivo_metadata(archivo_metadata, bloque_libre);

    // Logeamos operacion:
    log_info(logger, "Se compacto el archivo %s", get_nombre_archivo(archivo_metadata));
    log_info(logger, "Bloque inicial: %d", bloque_libre);
}