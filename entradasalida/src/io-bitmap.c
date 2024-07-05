#include "io-bitmap.h"

// Funcion para crear un bitmap:
t_bitarray *crear_bitmap(t_interfaz *interfaz, char *modo_de_apertura) {

    // Obtengo el archivo bitmap:
    FILE *archivo_bitmap = abrir_archivo_bitmap(interfaz, modo_de_apertura);
    int size = get_block_count(interfaz) / 8;

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

// Funcion para inicializar un bitmap:
void inicializar_bitmap(t_bitarray *bitmap) {
    int size = bitarray_get_max_bit(bitmap);

    for(int i = 0; i < size; i++)
        bitarray_clean_bit(bitmap, i);
}

// Funcion para obtener un bloque libre:
int obtener_bloque_libre(t_bitarray *bitmap, t_interfaz *interfaz) {

    // Obtengo la cantidad de bloques:
    int cantidad_bloques = get_block_count(interfaz);

    for(int i = 0; i < cantidad_bloques; i++) {
        if(!bitarray_test_bit(bitmap, i))
            return i;
    }

    return -1;
}

// Funciones para settear bloques como ocupados:
void set_bloque_ocupado(t_bitarray *bitmap, int bloque_inicial) {
    bitarray_set_bit(bitmap, bloque_inicial);
}

// Funcion para averiguar si hay los suficientes bloques libres:
int hay_suficientes_bloques_libres(t_bitarray *bitmap, t_interfaz *interfaz, int bloques_necesarios) {
    int cantidad_bloques = get_block_count(interfaz);
    int cantidad_bloques_libres = contar_bloques_libres(bitmap, 0, cantidad_bloques);

    return cantidad_bloques_libres >= bloques_necesarios;
}

// Funcion para contar bloques libres:
int contar_bloques_libres(t_bitarray *bitmap, int inicio, int fin) {
    int cantidad_bloques_libres = 0;

    for(int i = inicio; i < fin; i++) {
        if(!bitarray_test_bit(bitmap, i))
            cantidad_bloques_libres++;
    }

    return cantidad_bloques_libres;
}

// Funcion para averiguar si hay bloques contiguos libres:
int hay_bloques_contiguos_libres(t_bitarray *bitmap, int bloque_final, int bloques_necesarios) {

    // Verifico si hay bloques contiguos libres:
    for(int i = bloque_final + 1; i <= bloques_necesarios; i++) {
        if(bitarray_test_bit(bitmap, i))
            return 1; // Si no hay bloques contiguos libres, retorno 1
    }

    return 0; // Si hay bloques contiguos libres, retorno 0
}

// Funcion para setear bloques como ocupados:
void set_bloques_como_ocupados(t_bitarray *bitmap, int bloque_final, int bloques_necesarios) {
    int limite_de_bloques = bloque_final + bloques_necesarios;

    for(int i = bloque_final + 1; i <= limite_de_bloques; i++)
        set_bloque_ocupado(bitmap, i);
}
