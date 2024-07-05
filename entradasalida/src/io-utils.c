#include "io-utils.h"

//  Setea el socket con la memoria:
void set_socket_memory(int socket, t_interfaz * interfaz) {
    interfaz->socket_with_memoria = socket;
}

// Devuelve el socket con la memoria:
int get_socket_memory(t_interfaz * interfaz) {
    return interfaz->socket_with_memoria;
}

// Setea el socket con el kernel:
void set_socket_kernel(int socket, t_interfaz * interfaz) {
    interfaz->socket_with_kernel = socket;
}

// Devuelve el socket con el kernel:
int get_socket_kernel(t_interfaz *interfaz) {
    return interfaz->socket_with_kernel;
}

// Setea el nombre de la interfaz:
void set_interfaz_name(char* name, t_interfaz * interfaz) {
    interfaz->nombre = name;
}

// Devuelve el nombre de la interfaz:
char *get_interfaz_name(t_interfaz * interfaz) {
    return interfaz->nombre;
}

// Devuelve el tiempo de una interfaz:
int get_tiempo_unidad(t_interfaz* interfaz){
    return interfaz->config->TIEMPO_UNIDAD_TRABAJO;
}

// Devuelve el path de una interfaz:
char *get_path_dial_fs(t_interfaz *interfaz) {
    return interfaz->config->PATH_BASE_DIALFS;
}

// Devuelve el tipo de interfaz:
tipo_interfaz get_tipo_interfaz_to_int(t_interfaz *interfaz) {
    return interfaz->tipo;
}

// Devuelve el tamaño del bitmap:
size_t get_tamanio_bitmap(t_interfaz *interfaz) {
    int cantidad_bloques = get_block_count(interfaz);
    int tamanio_bloque = get_block_size(interfaz);

    return cantidad_bloques * tamanio_bloque;
}

// Devuelve la cantidad de bloques:
int get_block_count(t_interfaz *interfaz) {
    return interfaz->config->BLOCK_COUNT;
}

// Devuelve el tamaño de un bloque:
int get_block_size(t_interfaz *interfaz) {
    return interfaz->config->BLOCK_SIZE;
}

// Devuelve el total de bytes de una lista de direcciones:
int get_total_de_bytes(t_list *direcciones) {
    int size = list_size(direcciones);
    int bytes_totales = 0;

    for(int i = 0; i < size; i++) {
        t_direccion_fisica *direccion = list_get(direcciones, i);
        bytes_totales += direccion->tamanio;
    }

    return bytes_totales;
}

// Devuelve el nombre de una operación:
char *get_nombre_operacion(tipo_operacion operacion) {
    switch(operacion) {
        case IO_GEN_SLEEP_INT:
            return "IO_GEN_SLEEP";
            break;
        case IO_STDOUT_WRITE_INT:
            return "IO_STDOUT_WRITE";
            break;
        case IO_STDIN_READ_INT:
            return "IO_STDIN_READ";
            break;
        case IO_FS_CREATE_INT:
            return "IO_FS_CREATE";
            break;
        case IO_FS_READ_INT:
            return "IO_FS_READ";
            break;
        case IO_FS_WRITE_INT:
            return "IO_FS_WRITE";
            break;
        case IO_FS_DELETE_INT:
            return "IO_FS_DELETE";
            break;
        case IO_FS_TRUNCATE_INT:
            return "IO_FS_TRUNCATE";
            break;
        default:
            return "OPERACION DESCONOCIDA";
    }
}

// Devuelve si una dirección es mayor, menor o igual a otra:
int ordenar_direcciones_por_tamanio(void *direccion1, void *direccion2) {
    t_direccion_fisica *dir1 = direccion1;
    t_direccion_fisica *dir2 = direccion2;

    if (dir1->tamanio < dir2->tamanio) return -1;
    if (dir1->tamanio > dir2->tamanio) return 0;

    return 1;
}

// Devuelve el modo de apertura de un archivo:
char *get_modo_de_apertura(int size) {
    if(size == 0)
        return "wb+";
    else
        return "rb+";
}

// Devuelve el path de un archivo:
char *get_path_archivo(t_interfaz *interfaz, char *name_file) {
    char *path = string_new();

    string_append(&path, get_path_dial_fs(interfaz));
    string_append(&path, "/");
    string_append(&path, name_file);

    return path;
}

// Devuelve los bloques necesarios para un archivo:
int get_bloques_necesarios(t_interfaz *interfaz, int nuevo_tamanio) {
    int tamanio_bloque = get_block_size(interfaz);
    int bloques_necesarios = (int) ceil((double)nuevo_tamanio / tamanio_bloque);
    
    return bloques_necesarios;
}

// Funcion para calcular el bloque final:
int calcular_bloque_final(t_interfaz *interfaz, int bloque_inicial, int tamanio_archivo) {
    int bloques_necesarios = get_bloques_necesarios(interfaz, tamanio_archivo);

    return bloque_inicial + bloques_necesarios - 1;
}