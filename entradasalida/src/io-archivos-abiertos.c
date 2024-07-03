#include "io-archivos-abiertos.h"

t_list *obtener_archivos_ya_abiertos(t_interfaz *interfaz) {

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

            // Obtenemos el nombre del archivo:
            char *name_file = archivo->d_name;

            // Inicializamos el archivo abierto:
            t_config *archivo_metadata = abrir_archivo_metadata_config(interfaz, name_file, "r");
            t_archivo_abierto *archivo_abierto = malloc(sizeof(t_archivo_abierto));
            
            // Seteamos los datos del archivo abierto:
            set_archivo_metada_en_archivo_abierto(archivo_abierto, archivo_metadata);
            set_name_file_en_archivo_abierto(archivo_abierto, name_file);

            // Agregamos el archivo a la lista de archivos abiertos:
            if(es_un_archivo_valido(archivo_metadata))
                list_add(archivos_abiertos, archivo_abierto);
            else
                config_destroy(archivo_metadata);
        }
    }

    closedir(directorio);

    return archivos_abiertos;
}

// Funciones para setear los datos del archivo abierto:

void set_archivo_metada_en_archivo_abierto(t_archivo_abierto *archivo_abierto, t_config *archivo_metadata) {
    archivo_abierto->archivo_metadata = archivo_metadata;
}

void set_name_file_en_archivo_abierto(t_archivo_abierto *archivo_abierto, char *name_file) {
    archivo_abierto->name_file = name_file;
}

void set_nuevo_archivo_abierto(t_list *archivos_abiertos, char *name_file, t_config *archivo_metadata) {
    t_archivo_abierto *archivo_abierto = malloc(sizeof(t_archivo_abierto));
    archivo_abierto->name_file = name_file;
    archivo_abierto->archivo_metadata = archivo_metadata;

    list_add(archivos_abiertos, archivo_abierto);
}

// Funciones para gettear los datos del archivo abierto:

// Funciones para verificar si existe ya un archivo:

int ya_esta_abierto(t_list *archivos_abiertos, char *nombre_archivo) {
    int size = list_size(archivos_abiertos);

    for(int i = 0; i < size; i++) {
        t_archivo_abierto *archivo_abierto = list_get(archivos_abiertos, i);
        char *name_file = archivo_abierto->name_file;

        if(strcmp(name_file, nombre_archivo) == 0)
            return 1;
    }

    return 0;
}