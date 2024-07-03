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

    return archivo;
}

FILE *persistir_archivo(t_interfaz *interfaz, char *name_file, char *modo_de_apertura, int tamanio_archivo) {
    char *path = get_path_archivo(interfaz, name_file);
    FILE *archivo = abrir_archivo(path, modo_de_apertura);

    if(strcmp(modo_de_apertura, "wb+") == 0)
        ftruncate(fileno(archivo), tamanio_archivo);
    
    return archivo;
}

// Funciones de utilidad:

int es_un_archivo_txt(char *name_file) {
    return string_ends_with(name_file, ".txt");
}