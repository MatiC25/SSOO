#include "io-compactacion.h"

void compactar_fs(t_interfaz  *interfaz, FILE *bloques, t_bitarray *bitmap, t_list *archivos_ya_abiertos, t_config *archivo_metadata, int cantidad_bloques_asignados_a_archivo_compactar, int bloque_inicial_archivo_a_compactar, int tam_nuevo_archivo_a_compactar) {

    int cantidad_archivos_abiertos = list_size(archivos_ya_abiertos);

    t_list *archivos_ordenados = list_sorted(archivos_ya_abiertos, (void *) comparar_bloque_inicial);
    t_queue *buffers = queue_create();

    int bloque_final_anterior;
    int nuevo_bloque_libre;
    int bloque_inicial_archivo_actual;
    int tamanio_archivo_archivo_actual;
    int cantidad_bloques_asignados;
    unsigned char *buffer_archivo_a_compactar;

    for(int i =  0; i < cantidad_archivos_abiertos; i++) {
        t_archivo_abierto *archivo_abierto_actual = list_get(archivos_ordenados, i);
        t_config *archivo_metadata_archivo_actual = get_archivo_metadata(archivo_abierto_actual);

        bloque_inicial_archivo_actual = get_bloque_inicial(archivo_metadata_archivo_actual);
        tamanio_archivo_archivo_actual = get_tamanio_archivo(archivo_metadata_archivo_actual);
        nuevo_bloque_libre = obtener_bloque_libre(bitmap, interfaz);

        if(nuevo_bloque_libre < bloque_inicial_archivo_actual || bloque_inicial_archivo_actual == bloque_inicial_archivo_a_compactar) {   
            cantidad_bloques_asignados = calcular_cantidad_bloques_asignados(interfaz, tamanio_archivo_archivo_actual);

            liberar_bloques_asignados(bitmap, bloque_inicial_archivo_actual, cantidad_bloques_asignados);

            unsigned char *buffer = malloc(tamanio_archivo_archivo_actual);

            fseek(bloques, bloque_inicial_archivo_actual * get_block_size(interfaz), SEEK_SET);
            fread(buffer, tamanio_archivo_archivo_actual, 1, bloques);
            fseek(bloques, 0, SEEK_SET);
        
            if(bloque_inicial_archivo_actual != bloque_inicial_archivo_a_compactar) {
                bloque_final_anterior = calcular_bloque_final(interfaz, nuevo_bloque_libre, tamanio_archivo_archivo_actual);

                set_bloques_como_ocupados_desde(bitmap, nuevo_bloque_libre, cantidad_bloques_asignados);
                set_bloque_inicial_en_archivo_metadata(archivo_metadata_archivo_actual, nuevo_bloque_libre);
                set_archivo_metada_en_fs(archivo_metadata_archivo_actual);

                queue_push(buffers, buffer);
            } else {
                buffer_archivo_a_compactar = malloc(tamanio_archivo_archivo_actual);
                buffer_archivo_a_compactar = buffer;
            }
        }
    }

    queue_push(buffers, buffer_archivo_a_compactar);

    nuevo_bloque_libre = bloque_final_anterior + 1;
    cantidad_bloques_asignados = calcular_cantidad_bloques_asignados(interfaz, tam_nuevo_archivo_a_compactar);

    set_bloques_como_ocupados_desde(bitmap, nuevo_bloque_libre, cantidad_bloques_asignados);
    set_bloque_inicial_en_archivo_metadata(archivo_metadata, nuevo_bloque_libre);
}