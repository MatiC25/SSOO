#include "io-archivos-metadata.h"

t_config *abrir_archivo_metadata_config(t_interfaz *interfaz, char *name_file, char *modo_de_apertura) {
    char *path = get_path_archivo(interfaz, name_file);

    // Ya esta creado el archivo, solo lo abro:
    if(strcmp(modo_de_apertura, "r") == 0)
        return config_create(path);

    // Creo el archivo y lo abro:
    FILE *archivo = abrir_archivo(path, modo_de_apertura);
    t_config *config = config_create(path);

    fclose(archivo);
    return config;
}


// Funciones para validar los archivos:
int es_un_archivo_valido(t_config *archivo_metadata) {

    // Verificamos que el archivo tenga las propiedades necesarias:
    char *propiedades[] = {"BLOQUE_INICIAL", "TAMANIO_ARCHIVO", NULL};

    // Verificamos que el archivo tenga todas las propiedades necesarias:
    return tiene_todas_las_configuraciones(archivo_metadata, propiedades);
}

