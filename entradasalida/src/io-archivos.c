#include "io-archivos.h"

FILE *abrir_archivo_bloques(t_interfaz *interfaz, char *modo_de_apertura) {
    int tamanio_archivo = interfaz->config->BLOCK_SIZE * interfaz->config->BLOCK_COUNT;

    return persistir_archivo(interfaz, "bloques.bin", modo_de_apertura, tamanio_archivo);
}

FILE *abrir_archivo_metadata(t_interfaz *interfaz, char *name_file, char *modo_de_apertura) {
    char *path = get_path_archivo(interfaz, name_file);
    
    return abrir_archivo(path, modo_de_apertura);
}

FILE *abrir_archivo_bitmap(t_interfaz *interfaz, char *modo_de_apertura) {
    int tamanio_archivo = interfaz->config->BLOCK_COUNT / 8;

    return persistir_archivo(interfaz, "bitmap.bin", modo_de_apertura, tamanio_archivo);
}

FILE *abrir_archivo(char *path, char *modo_de_apertura) {
    FILE *archivo = fopen(path, modo_de_apertura);

    if(!archivo) {
        log_error(logger, "No se pudo abrir el archivo %s", path);
        return NULL;
    }

    // Liberamos la memoria utilizada:
    free(path);

    return archivo;
}

FILE *persistir_archivo(t_interfaz *interfaz, char *name_file, char *modo_de_apertura, int tamanio_archivo) {
    char *path = get_path_archivo(interfaz, name_file);
    FILE *archivo = abrir_archivo(path, modo_de_apertura);

    if(strcmp(modo_de_apertura, "wb+") == 0)
        ftruncate(fileno(archivo), tamanio_archivo);
    
    // Liberamos la memoria utilizada:
    free(path);

    return archivo;
}

// Funcion para escribir en un archivo:
void escribir_contenido_en_bloques(FILE *bloques, t_queue *buffers) {
    
    // Iteramos sobre los buffers:
    while(!queue_is_empty(buffers)) {
        unsigned char *buffer = queue_pop(buffers);
        int bytes_a_escribir = sizeof(buffer); 

        fwrite(buffer, bytes_a_escribir, 1, bloques);
        free(buffer);
    }

    // Liberamos la memoria utilizada:
    queue_destroy(buffers);
}