#include "io-utils-dial-fs.h"

// 1. Inicialización y configuración de archivos
FILE *iniciar_archivo(t_interfaz *interfaz, char *name_file) {
    char *path = build_full_path(interfaz, name_file);
    FILE *file = fopen(path, "rb+");

    if(!file)
        log_error(logger, "No se pudo abrir el archivo %s", path);
        
    return file;
}

FILE *iniciar_archivo_bloques(t_interfaz *interfaz) {
    return iniciar_archivo(interfaz, "bloques.dat");
}

FILE *iniciar_archivo_bitmap(t_interfaz *interfaz) {
    return iniciar_archivo(interfaz, "bitmap.dat");
}

t_config *iniciar_archivo_config(t_interfaz *interfaz, char *name_file) {
    char *path = build_full_path(interfaz, name_file);
    t_config *archivo_config = config_create(path);

    if(!archivo_config)
        log_error(logger, "No se pudo abrir el archivo %s", path);

    return archivo_config;
}

t_config *iniciar_archivo_metadata(t_interfaz *interfaz, char *name_file) {
    t_config *archivo_metadata = iniciar_archivo_config(interfaz, name_file);

    config_set_value(archivo_metadata, "TAMANIO_ARCHIVO", "0");

    return archivo_metadata;
}

t_bitarray *iniciar_bitmap(t_interfaz *interfaz) {
    FILE *bitmap_archivo = iniciar_archivo_bitmap(interfaz);

    size_t size = get_tamanio_bitmap(interfaz) / 8;
    void *bitmap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fileno(bitmap_archivo), 0);

    if(bitmap == MAP_FAILED) {
        log_info(logger, "No se pudo mapear el archivo bitmap");
        return NULL;
    }

    t_bitarray *bitarray = bitarray_create_with_mode(bitmap, size, MSB_FIRST);

    return bitarray;
}

// 2. Operaciones con bitmaps
void setear_bloque_ocupado(t_bitarray *bitmap, int bloque_libre) {
    int tamanio_bloque = get_block_size(interfaz);
    int bits_por_bloque = tamanio_bloque * 8;

    for(int i = 0; i < bits_por_bloque; i++) 
        bitarray_set_bit(bitmap, bloque_libre * bits_por_bloque + i);
}

int buscar_bloque_libre(t_bitarray *bitmap, t_interfaz *interfaz) {
    int cantidad_bloques = get_block_count(interfaz);
    int tamanio_bloque = get_block_size(interfaz);
    int bits_por_bloque = tamanio_bloque * 8;
    int bits_libres;

    for (int i = 0; i < cantidad_bloques; i++) {
        bits_libres = 0; 

        for (int j = 0; j < bits_por_bloque; j++)
            if (!bitarray_test_bit(bitmap, i * bits_por_bloque + j))
                bits_libres++;
    
        if(bits_libres == bits_por_bloque)
            return i;
    }

    return -1;
}

void liberar_bloques_usados(t_bitarray *bitmap, int bloque_inicial, int tam_archivo) {
    int tamanio_bloque = get_block_size(interfaz);
    int bits_por_bloque = tamanio_bloque * 8;
    int cantidad_bloques = tam_archivo / tamanio_bloque + 1;

    for(int i = 0; i < cantidad_bloques; i++) {
        for(int j = 0; j < bits_por_bloque; j++)
            bitarray_clean_bit(bitmap, (bloque_inicial + i) * bits_por_bloque + j);
    }    
}

void inicializar_bloques_vacios(t_bitarray *bitmap, t_interfaz *interfaz) {
    int cantidad_bloques = get_block_count(interfaz);
    int tamanio_bloque = get_block_size(interfaz); 
    int bits_por_bloque = tamanio_bloque * 8; 

    for(int i = 0; i < cantidad_bloques; i++) {
        for(int j = 0; j < bits_por_bloque; j++) 
            bitarray_clean_bit(bitmap, i * bits_por_bloque + j); 
    }
}

// 3. Operaciones con archivos de configuración:
int get_bloque_inicial(t_config *archivo_metadata) {
    return config_get_int_value(archivo_metadata, "BLOQUE_INICIAL");
}

int get_tamanio_archivo(t_config *archivo_metadata) {
    return config_get_int_value(archivo_metadata, "TAMANIO_ARCHIVO");
}

void set_bloque_libre_archivo_metadata(t_config *archivo_metadata, int bloque_libre) {
    char *bloque_libre_str = string_itoa(bloque_libre);

    config_set_value(archivo_metadata, "BLOQUE_INICIAL", bloque_libre_str);
    free(bloque_libre_str);
}

// 4. Operaciones de lectura y escritura:
void procesar_operacio_de_lectura_o_escritura(t_config *archivo_metadata, t_list *argumentos, int *bloque_inicial, int *tamanio_archivo, int *bytes_a_leer, int *offset, int *dirreccion_fisica) {
    *bloque_inicial = get_bloque_inicial(archivo_metadata);
    *tamanio_archivo = get_tamanio_archivo(archivo_metadata);
    *bytes_a_leer = list_get(argumentos, 1);
    *offset = list_get(argumentos, 2);
    *dirreccion_fisica = list_get(argumentos, 3);
}

void leer_contenido_desde_bloques(FILE *bloques, int bloque_inicial, int offset, int bytes_a_leer, unsigned char *buffer, t_interfaz *interfaz) {
    fseek(bloques, bloque_inicial * get_block_size(interfaz) + offset, SEEK_SET);
    fread(buffer, bytes_a_leer, 1, bloques);
    fseek(bloques, 0, SEEK_SET); 
}

void escribir_contenido_en_bloques(FILE *bloques, int bloque_inicial, int offset, int tamanio_archivo, unsigned char *contenido, int bytes_a_escribir, t_interfaz *interfaz) {
    int posicion_actual_en_bloque = bloque_inicial * get_block_size(interfaz) + offset;
    int limite_bloques = bloque_inicial * get_block_size(interfaz) + tamanio_archivo; 
    int cantidad_de_bloques_restantes = (limite_bloques - posicion_actual_en_bloque) / get_block_size(interfaz);
    int cantidad_bytes_restantes = cantidad_de_bloques_restantes * get_block_size(interfaz);

    fseek(bloques, posicion_actual_en_bloque, SEEK_SET);

    while(bytes_a_escribir > 0 && bytes_a_escribir <= cantidad_bytes_restantes) {
        int bytes_escritos = fwrite(contenido, sizeof(unsigned char), 1, bloques);
        bytes_a_escribir -= bytes_escritos;
        contenido += bytes_escritos;
    }

    fseek(bloques, 0, SEEK_SET);
}

// 5. Utilidades
t_config *get_archivo_config_from_args(t_interfaz *interfaz, t_list *argumentos) {
    char *name_file = list_get(argumentos, 0);

    return iniciar_archivo_config(interfaz, name_file);
}

char *build_full_path(t_interfaz *interfaz, const char *name_file) {
    char *path_bloques = get_path_dial_fs(interfaz);

    if (!path_bloques || !name_file) 
        return NULL;

    int tam_new_string = strlen(path_bloques) + strlen(name_file) + 1;
    char *path_completo = malloc(tam_new_string);

    if (!path_completo) 
        return NULL;

    strcpy(path_completo, path_bloques);
    strcat(path_completo, name_file);

    return path_completo;
}

int es_un_archivo_valido(t_config *archivo_metada) {
    char *propiedades[] = {
        "TAMANIO_ARCHIVO",
        "BLOQUE_INICIAL",
        NULL
    };

    return tiene_todas_las_configuraciones(archivo_metada, propiedades);
}