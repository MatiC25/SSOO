#include "io-compactacion.h"

void compactar_archivos(t_bitarray *bitmap, FILE *bloques, t_interfaz *interfaz, t_list *archivos_abiertos, t_config *archivo_metadata, int cantidad_de_bloques_nuevos_necesarios) {

    // Creamos una lista de buffers y ordenamos los archivos abiertos:
    t_list *buffers = list_create();
    t_list *archivos_ordenados = list_sorted(archivos_abiertos, (void *) ordenar_por_bloque_inicial);

    // Obtenemos el bloque inicial del archivo a compactar y su tamaño:
    int size = list_size(archivos_ordenados);
    int bloque_del_archivo_a_compactar = get_bloque_inicial(archivo_metadata);
    int tamanio_archivo = get_tamanio_archivo(archivo_metadata);

    for (int i = 0; i < size; i++) {
        // Obtenemos el archivo abierto y su metadata:
        t_archivo_abierto *archivo_abierto = list_get(archivos_ordenados, i);
        t_config *archivo_metadata = get_archivo_metadata(archivo_abierto);

        // Obtenemos el nuevo bloque inicial y el bloque inicial actual del archivo:
        int bloque_inicial = get_bloque_inicial(archivo_metadata);
        int bloque_libre = buscar_bloque_libre(bitmap, interfaz);

        if (bloque_inicial == bloque_del_archivo_a_compactar || bloque_libre < bloque_inicial) {
            // Liberar bloques usados por el archivo:
            liberar_bloques_usados(bitmap, bloques, archivo_metadata);

            // Calculamos la cantidad de bytes que debe de tener el buffer:
            unsigned char *buffer = malloc(cantidad_de_bloques_necesarios * get_tamanio_bloque(interfaz));

            // Leer contenido de los bloques:
            fseek(bloques, bloque_inicial * get_tamanio_bloque(interfaz), SEEK_SET);
            fread(buffer, get_tamanio_bloque(interfaz), cantidad_de_bloques_necesarios, bloques);
            fseek(bloques, 0, SEEK_SET);

            // Agregar buffer a la lista
            list_add(buffers, buffer);

            // Asignar nuevo bloque inicial si es necesario
            if (bloque_inicial != bloque_del_archivo_a_compactar) {
                set_bloque_inicial_archivo_metadata(archivo_metadata, bloque_libre);
                asignar_bloques_nuevos_desde_inicio(bitmap, interfaz, archivo_metadata, cantidad_de_bloques_necesarios);
            } else {
                liberar_bloques_usados(bitmap, bloque_del_archivo_a_compactar, tamanio_archivo);
            }
        }
    }

    // Asignar bloques nuevos al archivo
    int bloque_libre_final = buscar_bloque_libre(bitmap, interfaz);
    set_bloque_inicial_archivo_metadata(archivo_metadata, bloque_libre_final);
    asignar_bloques_nuevos_desde_inicio(bitmap, interfaz, archivo_metadata, cantidad_de_bloques_necesarios);
    asignar_nuevos_bloques_desde_final(bitmap, archivo_metadata, cantidad_de_bloques_necesarios);

    // Escribir buffers en bloques
    escribir_buffers_en_bloques(bloques, buffers);
}
