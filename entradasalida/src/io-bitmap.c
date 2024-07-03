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
    t_bitarray *bitarray = bitarray_create_with_mode(bitmap, size, LSB_FIRST);

    if(strcmp(modo_de_apertura, "wb+") == 0)
        inicializar_bitmap(bitarray);

    return bitarray;
}

void inicializar_bitmap(t_bitarray *bitmap) {
    int size = bitarray_get_max_bit(bitmap);

    for(int i = 0; i < size; i++)
        bitarray_clean_bit(bitmap, i);
}

// Funcion para obtener un bloque libre:
int obtener_bloque_libre(t_bitarray *bitmap, t_interfaz *interfaz) {

    // Obtenemos la cantidad de bloques y la cantidad de bits por bloque:
    int cantidad_bloques = get_block_count(interfaz);
    int bits_por_bloque = cantidad_bloques / 8;
    int cantidad_bits;

    for(int i = 0; i < cantidad_bloques; i++) {

        // Obtenemos el bloque:
        int bloque = i * bits_por_bloque;
        cantidad_bits = 0;

        // Verificamos si el bloque esta libre:
        for(int j = 0; j < bits_por_bloque; j++) {
            if(!bitarray_test_bit(bitmap, bloque + j)) {
                cantidad_bits++;
            }
        }

        // Si el bloque esta libre, lo devolvemos:
        if(cantidad_bits == bits_por_bloque)
            return i;
    }

    return -1;
}

// Funciones para settear bloques como ocupados:
void set_bloque_ocupado(t_bitarray *bitmap, t_interfaz *interfaz, int bloque_inicial) {
    int cantidad_bits_por_bloque = get_block_count(interfaz) / 8;

    for(int i = 0; i < cantidad_bits_por_bloque; i++) 
        bitarray_set_bit(bitmap, bloque_inicial + i);
}