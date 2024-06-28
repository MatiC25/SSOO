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

t_list *traer_archivos_abiertos(t_interfaz *interfaz) {

    // Obtenemos el directorio: 
    char *path_directorio = get_path_dial_fs(interfaz);
    DIR *directorio = opendir(path_directorio);

    if(!directorio) {
        log_error(logger, "No se pudo abrir el directorio %s", path_directorio);
        return NULL;
    }

    // Inicializamos la lista de archivos:
    t_list *archivos_abiertos = list_create();

    // Iteramos sobre los archivos del directorio:
    struct dirent *archivo;
    while(archivo = readdir(directorio)) {
        if(archivo->d_type == DT_REG) { // Si es un archivo regular
            char *name_file = archivo->d_name;
            t_config *archivo_metadata = iniciar_archivo_config(interfaz, name_file);
            t_archivo_abierto *archivo_abierto = malloc(sizeof(t_archivo_abierto));
            
            // Seteamos los datos del archivo abierto:
            set_archivo_metada_en_archivo_abierto(archivo_abierto, archivo_metadata);
            set_name_file_en_archivo_abierto(archivo_abierto, name_file);

            if(es_un_archivo_valido(archivo_metadata))
                list_add(archivos_abiertos, archivo_abierto);
            else
                config_destroy(archivo_metadata);
        }
    }

    closedir(directorio);

    return archivos_abiertos;
}

int existe_archivo_abierto(t_list *archivos_abiertos, char *name_file) {

    // Iteramos sobre los archivos abiertos:
    for(int i = 0; i < list_size(archivos_abiertos); i++) {
        t_archivo_abierto *archivos_abierto = list_get(archivos_abiertos, i);
        char *name_file_abierto = get_name_file(archivos_abierto);

        if(strcmp(name_file, name_file_abierto) == 0)
            return 1;
    }

    return 0;
}

void agregar_a_archivos_abiertos(t_list *archivos_abiertos, t_config *archivo_metadata, char *name_file, int bloque_inicial) {
    t_archivo_abierto *archivo_abierto = malloc(sizeof(t_archivo_abierto));

    set_archivo_metada_en_archivo_abierto(archivo_abierto, archivo_metadata);
    set_name_file_en_archivo_abierto(archivo_abierto, name_file);
    set_bloque_inicial_en_archivo_abierto(archivo_abierto, bloque_inicials);

    list_add(archivos_abiertos, archivo_abierto);
}

void set_archivo_metada_en_archivo_abierto(t_archivo_abierto *archivo_abierto, t_config *archivo_metadata) {
    archivo_abierto->archivo_metadata = archivo_metadata;
}

void set_name_file_en_archivo_abierto(t_archivo_abierto *archivo_abierto, char *name_file) {
    archivo_abierto->name_file = name_file;
}

void set_bloque_inicial_en_archivo_abierto(int bloque_inicial, t_archivo_abierto *archivo_abierto) {
    archivo_abierto->bloque_inicial = bloque_inicial;
}

void agregar_a_archivos_abiertos(t_list *archivos_abiertos, t_config *archivo_metadata) {
    list_add(archivos_abiertos, archivo_metadata);
}

char *get_name_file(t_archivo_abierto *archivo_abierto) {
    return archivo_abierto->name_file;
}

t_archivo_abierto *get_archivo_abierto(t_list *archivos_abiertos, char *name_file) {

    // Iteramos sobre los archivos abiertos:
    for(int i = 0; i < list_size(archivos_abiertos); i++) {
        t_archivo_abierto *archivo_abierto = list_get(archivos_abiertos, i);
        char *name_file_abierto = get_name_file(archivo_abierto);

        if(strcmp(name_file, name_file_abierto) == 0)
            return archivo_abierto;
    }

    return NULL;
}

void eliminar_metadata_archivo(t_config *archivo_metadata) {
    remove(archivo_metadata->path);
    config_destroy(archivo_metadata);
}

void crear_archivo(t_config *archivo_metada, t_interfaz *interfaz, char *name_file) {
    char *path_completo = build_full_path(interfaz, name_file);
    config_save_in_file(archivo_metada, path_completo); // Guardamos el archivo
}

void get_archivo_metadata(t_archivo_abierto *archivo_abierto) {
    return archivo_abierto->archivo_metadata;
}

void eliminar_archivo_de_archivos_abiertos(t_list *archivos_abiertos, char *name_file) {
    int index = 0;
    int size = list_size(archivos_abiertos);

    while(index > size && strcmp(get_name_file(list_get(archivos_abiertos, index)), name_file) != 0)
        index++;

    if(index < size) 
        list_remove(archivos_abiertos, index);
}

// 2. Operaciones con bitmaps
void setear_bloque_ocupado(t_bitarray *bitmap, int bloque_libre) {
    int tamanio_bloque = get_block_size(interfaz);
    int bits_por_bloque = tamanio_bloque * 8;

    for(int i = 0; i < bits_por_bloque; i++) 
        bitarray_set_bit(bitmap, bloque_libre * bits_por_bloque + i);
}

int preguntar_si_bloque_esta_ocupado(t_bitarray *bitmap, int bloque_a_consultar) {
    int tamanio_bloque = get_block_size(interfaz);
    int bits_por_bloque = tamanio_bloque * 8;

    for(int i = 0; i < bits_por_bloque; i++) 
        if(!bitarray_test_bit(bitmap, bloque_a_consultar * bits_por_bloque + i))
            return 0; // Si alguno de los bits esta ocupado, devolvemos 0

    return 1; // Si todos los bits estan libres, devolvemos 1
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
    int cantidad_bloques = tam_archivo / tamanio_bloque + (tam_archivo % tamanio_bloque != 0 ? 1 : 0); // Redondeamos hacia arriba

    for(int i = 0; i < cantidad_bloques; i++) {
        for(int j = 0; j < bits_por_bloque; j++) {
            // Calculamos el índice del bit:
            int bit_index = (bloque_inicial + i) * bits_por_bloque + j;

            if(bit_index < tam_archivo * 8) 
                bitarray_clean_bit(bitmap, bit_index);
        }
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

int hay_bloques_suficientes(t_bitarray *bitmap, t_interfaz *interfaz, int cantidad_de_bloques_nuevos_necesarios) {
    int cantidad_bloques = get_block_count(interfaz);
    int bloques_libres = contar_bloques_libres_hasta(bitmap, 0, cantidad_bloques);

    return bloques_libres >= cantidad_de_bloques_nuevos_necesarios;
}

int hay_bloques_contiguos(t_bitarray *bitmap, t_interfaz *interfaz, t_config *archivo_metadata, int cantidad_de_bloques_nuevos_necesarios) {
    // Determinamos el bloque final:
    int bloque_final = calcular_bloque_final(interfaz, archivo_metadata);
    int cantidad_bloques = get_block_count(interfaz);

    // Nos paramos en el siguiente bloque del bloque final:
    int siguiente_bloque = bloque_final + 1; 
    int bloques_contiguos = contar_bloques_libres_hasta(bitmap, siguiente_bloque, cantidad_bloques);

    return bloques_contiguos >= cantidad_de_bloques_nuevos_necesarios;
}


void asignar_nuevos_bloques_desde_final(t_bitarray *bitmap, t_interfaz *interfaz, t_config *archivo_metadata, int cantidad_de_bloques_nuevos_necesarios) {

    // Determinamos el bloque final:
    int bloque_final = calcular_bloque_final(interfaz, archivo_metadata);
    int cantidad_bloques = get_block_count(interfaz);

    // Nos paramos en el siguiente bloque del bloque final:
    int siguiente_bloque = bloque_final + 1; 

    while(siguiente_bloque >= cantidad_bloques && cantidad_de_bloques_nuevos_necesarios > 0) {
        // Asignamos el bloque:
        setear_bloque_ocupado(bitmap, siguiente_bloque);

        // Nos movemos al siguiente bloque:
        siguiente_bloque++;

        // Disminuimos la cantidad de bloques necesarios:
        cantidad_de_bloques_nuevos_necesarios--;
    }
}

void asignar_bloques_nuevos_desde_inicio(t_bitarray *bitmap, t_interfaz *interfaz, t_config *archivo_metadata, int cantidad_de_bloques_nuevos_necesarios) {
    int cantidad_bloques = get_block_count(interfaz);
    int tamanio_bloque = get_block_size(interfaz);
    int bloque_inicial = get_bloque_inicial(archivo_metadata);

    for(int i = 0; i < cantidad_de_bloques_necesarios; i++) {
        // Asignamos el bloque:
        setear_bloque_ocupado(bitmap, bloque_inicial + i);
    }
}

int contar_bloques_libres_hasta(t_bitarray *bitmap, int desde_bloque, int hasta_bloque) {
    int bloques_libres = 0;

    for(int i = desde_bloque; i <= hasta_bloque; i++)   
        if(preguntar_si_bloque_esta_ocupado(bitmap, i))
            bloques_libres++;

    return bloques_libres;
}

int calcular_bloque_final(t_interfaz *interfaz, t_archivo_metadata *archivo_metadata) {
    int cantidad_bloques = get_block_count(interfaz);
    int tamanio_bloque = get_block_size(interfaz);

    // Obtenemos los datos del archivo:
    int bloque_inicial = get_bloque_inicial(archivo_metadata);
    int tamanio_archivo = get_tamanio_archivo(archivo_metadata);

    // Calculamos la cantidad de bloques del archivo:
    int cantidad_de_bloques_del_archivo = (tamanio_archivo + tamanio_bloque - 1) / tamanio_bloque;

    // Determinamos y retornamos el bloque final:
    return bloque_inicial + cantidad_de_bloques_del_archivo - 1;
}

int calcular_cantidad_bloques_necesarios(t_interfaz *interfaz, int tam_a_establecer) {
    int tamanio_bloque = get_block_size(interfaz);
    return tam_a_establecer / tamanio_bloque + (tam_a_establecer % tamanio_bloque != 0 ? 1 : 0);
}

// 3. Operaciones con archivos de configuración:
void set_tamanio_archivo(t_config *archivo_metadata, int tam_a_establecer) {
    char *tamanio_archivo_str = string_itoa(tam_a_establecer);

    config_set_value(archivo_metadata, "TAMANIO_ARCHIVO", tamanio_archivo_str);
    free(tamanio_archivo_str);
}

void set_bloque_inicial_archivo_metadata(t_config *archivo_metadata, int bloque_inicial) {
    char *bloque_inicial_str = string_itoa(bloque_inicial);

    config_set_value(archivo_metadata, "BLOQUE_INICIAL", bloque_inicial_str);
    free(bloque_inicial_str);
}

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

void liberar_recuros_archivo(t_bitarray* bitmap, t_config* archivo_metadata) {

    // Obtenemos los datos del archivo:
    int bloque_inicial = get_bloque_inicial(archivo_metadata);
    int tamanio_archivo = get_tamanio_archivo(archivo_metadata);

    // Liberamos los bloques usados:
    liberar_bloques_usados(bitmap, bloque_inicial, tamanio_archivo);
}

int calcular_cantidad_de_bloques_asignados(t_interfaz *interfaz, t_archivo_metadata *archivo_metadata) {
    int tamanio_archivo = get_tamanio_archivo(archivo_metadata);
    int tamanio_bloque = get_block_size(interfaz);

    return (tamanio_archivo + tamanio_bloque - 1) / tamanio_bloque;
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

void ordenar_por_bloque_inicial(t_archivo_abierto *archivo_abierto, t_archivo_abierto *otro_archivo_abierto) {
    return get_bloque_inicial(archivo_abierto) - get_bloque_inicial(otro_archivo_abierto);
}

void escribir_buffers_en_bloques(FILE *bloques, t_list *buffers) {
    int cantidad_buffers = list_size(buffers);

    unsigned char *buffer_primero = list_get(buffers, 0); 
    int tamanio_buffer = sizeof(buffer_primero);

    fseek
}