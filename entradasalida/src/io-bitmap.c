#include "io-bitmap.h"

t_bitarray *crear_bitmap(t_interfaz *interfaz, char *modo_de_apertura) {

    // Obtengo el archivo bitmap:
    FILE *archivo_bitmap = abrir_archivo_bitmap(interfaz, modo_de_apertura);
    int size = get_block_count(interfaz);

    // Mapeo el archivo bitmap:
    void *bitmap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(archivo_bitmap), 0);

    // Verifico que se haya mapeado correctamente:
    if(bitmap == MAP_FAILED) {
        log_info(logger, "No se pudo mapear el archivo bitmap");
        return NULL;
    }

    // Creo el bitmap:
    t_bitarray *bitarray = bitarray_create_with_mode(bitmap, size, MSB_FIRST);

    if(strcmp(modo_de_apertura, "wb+") == 0)
        inicializar_bitmap(bitarray);

    return bitmap;
}

void inicializar_bitmap(t_bitarray *bitmap) {
    int size = bitarray_get_max_bit(bitmap);

    for(int i = 0; i < size; i++)
        bitarray_clean_bit(bitmap, i);
}

