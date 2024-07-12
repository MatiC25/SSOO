#include "io-archivos.h"

FILE *abrir_archivo_metadata(t_interfaz *interfaz, char *name_file, char *modo_de_apertura) {
    char *path = get_path_archivo(interfaz, name_file);
    FILE *archivo = abrir_archivo(path, modo_de_apertura);

    // Liberar la memoria del path:
    free(path); 
    
    return archivo;
}

FILE *abrir_archivo_bitmap(t_interfaz *interfaz, char *modo_de_apertura) {
    int tamanio_archivo = interfaz->config->BLOCK_COUNT / 8;
    FILE *archivo = persistir_archivo(interfaz, "bitmap.bin", modo_de_apertura, tamanio_archivo);

    return archivo;
}

FILE *abrir_archivo_bloques(t_interfaz *interfaz, char *modo_de_apertura) {
    int tamanio_archivo = interfaz->config->BLOCK_SIZE * interfaz->config->BLOCK_COUNT;
    FILE *archivo = persistir_archivo(interfaz, "bloques.bin", modo_de_apertura, tamanio_archivo);

    return archivo;
}

FILE *persistir_archivo(t_interfaz *interfaz, char *name_file, char *modo_de_apertura, int tamanio_archivo) {
    char *path = get_path_archivo(interfaz, name_file);
    FILE *archivo = abrir_archivo(path, modo_de_apertura);

    if (archivo && strcmp(modo_de_apertura, "wb+") == 0) {
        ftruncate(fileno(archivo), tamanio_archivo);
    }

    // Liberar la memoria del path:
    free(path); 

    return archivo;
}

FILE *abrir_archivo(char *path, char *modo_de_apertura) {
    FILE *archivo = fopen(path, modo_de_apertura);

    if (!archivo) {
        log_error(logger, "No se pudo abrir el archivo %s", path);
        return NULL;
    }

    return archivo;
}

// Funcion para escribir en un archivo:
void escribir_contenido_en_bloques(FILE *bloques, t_queue *buffers) {
    
    // Iteramos sobre los buffers:
    while(!queue_is_empty(buffers)) {
        // Obtenemos el buffer y la cantidad de bytes a escribir:
        unsigned char *buffer = queue_pop(buffers);
        int *bytes_a_escribir = queue_pop(buffers);

        fwrite(buffer, *bytes_a_escribir, 1, bloques);

        // Liberamos la memoria utilizada:
        free(buffer);
        free(bytes_a_escribir);
    }

    // Liberamos la memoria utilizada:
    queue_destroy(buffers);
}