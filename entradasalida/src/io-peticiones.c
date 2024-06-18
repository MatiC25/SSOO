#include "io-peticiones.h"

void interfaz_recibir_peticiones(t_interfaz * interfaz) {
    if(interfaz->tipo == GENERICA)
        ejecutar_operacion_generica(interfaz);
    else if(interfaz->tipo == STDIN)
        ejecutar_operacion_stdin(interfaz);
    else if(interfaz->tipo == STDOUT)
        ejecutar_operacion_stdout(interfaz);
    else
        log_error(logger, "Tipo de interfaz no soportado");
}

void ejecutar_operacion_generica(t_interfaz * interfaz) {
    // Inicializamos las variables:
    int tiempo_espera;
    int tiempo_unidad;

    while(1) {
        tiempo_espera = recibir_entero(interfaz);
        tiempo_unidad = get_tiempo_unidad(interfaz);

        // Realizamos la operacion:
        sleep(tiempo_espera * tiempo_unidad);

        // Enviamos la respuesta:
        send_respuesta_a_kernel(1, interfaz);
    }
}

void ejecutar_operacion_stdin(t_interfaz *interfaz) {
    // Inicializamos las variables:
    char *input = NULL;

    while(1) {
        input = readline("Ingrese un valor: ");

        if(input == NULL) {
            log_error(logger, "Error al leer el input");
            exit(EXIT_FAILURE);
        }

        // Realizamos la operacion:
        int direccion_fisica = recibir_entero(interfaz);
        int bytes_a_leer = recibir_entero(interfaz);

        // Tomamos los bystes a leer de input:
        char *bytes = malloc(bytes_a_leer);
        memcpy(bytes, input, bytes_a_leer);

        // Enviamos los bytes a memoria:
        send_bytes_a_grabar(interfaz, direccion_fisica, bytes, bytes_a_leer);
        send_respuesta_a_kernel(1, interfaz);

        free(input);
        free(bytes);
    }
}

void ejecutar_operacion_stdout(t_interfaz *interfaz) {
    while(1) {
        int direccion_fisica = recibir_entero(interfaz);
        int bytes_a_mostrar = recibir_entero(interfaz);
        
        // Instanciamos los bytes a mostrar:
        char *bytes = malloc(bytes_a_mostrar);

        // Solicitamos los bytes a memoria:
        send_bytes_a_leer(interfaz, direccion_fisica, bytes_a_mostrar);
        rcv_bytes_a_leer(interfaz, bytes_a_mostrar, &bytes);

        // Mostramos los bytes:
        log_info(logger, "Bytes a mostrar: %s", bytes);

        // Enviamos la respuesta:
        send_respuesta_a_kernel(1, interfaz);

        free(bytes);
    }
}

void ejecutar_operacion_dialfs(t_interfaz* interfaz){
    while (1) {
        int operacion = recibir_entero(interfaz); //hay que añadir un enum con los casos pedidos
                                                  //cambiar el tipo de "operacion"
        switch (operacion){
            case(IO_FS_CREATE):
                crear_fs();
                break;
            case(IO_FS_DELETE):
                borrar_fs();
                break;
            case(IO_FS_TRUNCATE):
                truncar_fs();
                break;
            case(IO_FS_WRITE):
                escribir_fs();
                break;
            case(IO_FS_READ):
                leer_fs();
            default:
                log_error(logger, "Operación no soportada en DialFS");
                break;
        }
    }
}

void crear_fs(t_interfaz *interfaz){
    char *nombre_archivo = recibir_string(interfaz);
    int bloque_inicial = obtener_bloque_libre();
    
    if (bloque_inicial == -1) {
        log_error(logger, "No hay bloques disponibles");
        free(nombre_archivo);
        return;
    }
    
    marcar_bloque_ocupado(bloque_inicial);
    crear_metadata_archivo(nombre_archivo, bloque_inicial, 0);
    log_info(logger, "Archivo %s creado en el bloque %d", nombre_archivo, bloque_inicial);
    
    free(nombre_archivo);
    send_respuesta_a_kernel(1, interfaz);
}

void borrar_fs(t_interfaz *interfaz){
    char *nombre_archivo = recibir_string(interfaz);
    int bloque_inicial, tamanio_archivo;
    
    if (!leer_metadata_archivo(nombre_archivo, &bloque_inicial, &tamanio_archivo)){
        log_error(logger, "No se encontró el archivo %s", nombre_archivo);
        free(nombre_archivo);
        return;
    }
    
    liberar_bloques(bloque_inicial, tamanio_archivo);
    eliminar_metadata_archivo(nombre_archivo);
    log_info(logger, "Archivo %s eliminado", nombre_archivo);
    
    free(nombre_archivo);
    send_respuesta_a_kernel(1, interfaz);
}

void truncar_fs(t_interfaz *interfaz){
    char *nombre_archivo = recibir_string(interfaz);
    int nuevo_tamanio = recibir_entero(interfaz);
    
    int bloque_inicial, tamanio_actual;
    if (!leer_metadata_archivo(nombre_archivo, &bloque_inicial, &tamanio_actual)){
        log_error(logger, "No se encontró el archivo %s", nombre_archivo);
        free(nombre_archivo);
        return;
    }
    
    if (nuevo_tamanio > tamanio_actual){
        int bloques_necesarios = (nuevo_tamanio + BLOCK_SIZE - 1) / BLOCK_SIZE;
        if (!hay_espacio_para_bloques(bloques_necesarios)) {
            compactar_fs();
        }
        asignar_bloques_adicionales(bloque_inicial, nuevo_tamanio);
    } else {
        liberar_bloques(bloque_inicial + nuevo_tamanio, tamanio_actual - nuevo_tamanio);
    }
    
    actualizar_metadata_archivo(nombre_archivo, bloque_inicial, nuevo_tamanio);
    log_info(logger, "Archivo %s truncado a %d bytes", nombre_archivo, nuevo_tamanio);
    
    free(nombre_archivo);
    send_respuesta_a_kernel(1, interfaz);
}

void escribir_fs(t_interfaz *interfaz){
    char *nombre_archivo = recibir_string(interfaz);
    int direccion_fisica = recibir_entero(interfaz);
    int bytes_a_escribir = recibir_entero(interfaz);
    
    char *datos = malloc(bytes_a_escribir);
    recibir_datos(interfaz, datos, bytes_a_escribir);
    
    int bloque_inicial, tamanio_actual;
    if (!leer_metadata_archivo(nombre_archivo, &bloque_inicial, &tamanio_actual)){
        log_error(logger, "No se encontró el archivo %s", nombre_archivo);
        free(nombre_archivo);
        free(datos);
        return;
    }
    
    escribir_datos_en_bloques(bloque_inicial, direccion_fisica, datos, bytes_a_escribir);
    if (direccion_fisica + bytes_a_escribir > tamanio_actual){
        actualizar_metadata_archivo(nombre_archivo, bloque_inicial, direccion_fisica + bytes_a_escribir);
    }
    
    log_info(logger, "Escribiendo %d bytes en el archivo %s", bytes_a_escribir, nombre_archivo);
    
    free(nombre_archivo);
    free(datos);
    send_respuesta_a_kernel(1, interfaz);
}

void leer_fs(t_interfaz *interfaz){
    char *nombre_archivo = recibir_string(interfaz);
    int direccion_fisica = recibir_entero(interfaz);
    int bytes_a_leer = recibir_entero(interfaz);
    
    int bloque_inicial, tamanio_archivo;
    if (!leer_metadata_archivo(nombre_archivo, &bloque_inicial, &tamanio_archivo)){
        log_error(logger, "No se encontró el archivo %s", nombre_archivo);
        free(nombre_archivo);
        return;
    }
    
    if (direccion_fisica + bytes_a_leer > tamanio_archivo){
        log_error(logger, "Lectura fuera de los límites del archivo %s", nombre_archivo);
        free(nombre_archivo);
        return;
    }
    
    char *datos = malloc(bytes_a_leer);
    leer_datos_de_bloques(bloque_inicial, direccion_fisica, datos, bytes_a_leer);
    
    log_info(logger, "Leyendo %d bytes del archivo %s", bytes_a_leer, nombre_archivo);
    
    free(nombre_archivo);
    free(datos);
    send_respuesta_a_kernel(1, interfaz);
}

void marcar_bloque_ocupado(int bloque) {
    FILE *bitmap_file = fopen(PATH_BITMAP, "rb+");
    if (!bitmap_file) {
        log_error(logger, "No se pudo abrir el archivo bitmap");
        return;
    }

    uint8_t bitmap[BLOCK_COUNT / 8];
    fread(bitmap, sizeof(bitmap), 1, bitmap_file);

    bitmap[bloque / 8] |= (1 << (bloque % 8));

    fseek(bitmap_file, 0, SEEK_SET);
    fwrite(bitmap, sizeof(bitmap), 1, bitmap_file);
    fclose(bitmap_file);
}


Para implementar las funciones auxiliares mencionadas, es esencial manejar archivos y operaciones de I/O de manera eficiente. A continuación, se presentan las implementaciones de las funciones auxiliares necesarias para gestionar el sistema de archivos simulado.

Definiciones y Constantes
Primero, definimos algunas constantes y estructuras necesarias para el manejo de los archivos y el bitmap:

c
Copiar código
#define BLOCK_SIZE 1024
#define BLOCK_COUNT 1000
#define PATH_BLOQUES "bloques.dat"
#define PATH_BITMAP "bitmap.dat"
#define PATH_METADATA "./metadata/"

// Puedes ajustar estos valores y rutas según sea necesario

typedef struct {
    int bloque_inicial;
    int tamanio_archivo;
} metadata_archivo_t;
Funciones Auxiliares
Obtener un bloque libre
c
Copiar código
int obtener_bloque_libre() {
    FILE *bitmap_file = fopen(PATH_BITMAP, "rb+");
    if (!bitmap_file) {
        log_error(logger, "No se pudo abrir el archivo bitmap");
        return -1;
    }

    uint8_t bitmap[BLOCK_COUNT / 8];
    fread(bitmap, sizeof(bitmap), 1, bitmap_file);

    for (int i = 0; i < BLOCK_COUNT; i++) {
        if (!(bitmap[i / 8] & (1 << (i % 8)))) {
            fclose(bitmap_file);
            return i;
        }
    }

    fclose(bitmap_file);
    return -1;
}
Marcar un bloque como ocupado
c
Copiar código
void marcar_bloque_ocupado(int bloque) {
    FILE *bitmap_file = fopen(PATH_BITMAP, "rb+");
    if (!bitmap_file) {
        log_error(logger, "No se pudo abrir el archivo bitmap");
        return;
    }

    uint8_t bitmap[BLOCK_COUNT / 8];
    fread(bitmap, sizeof(bitmap), 1, bitmap_file);

    bitmap[bloque / 8] |= (1 << (bloque % 8));

    fseek(bitmap_file, 0, SEEK_SET);
    fwrite(bitmap, sizeof(bitmap), 1, bitmap_file);
    fclose(bitmap_file);
}

void crear_metadata_archivo(char* nombre, int bloque_inicial, int tamanio) {
    char path[256];
    sprintf(path, "%s%s", PATH_METADATA, nombre);

    FILE *metadata_file = fopen(path, "w");
    if (!metadata_file) {
        log_error(logger, "No se pudo crear el archivo de metadata %s", nombre);
        return;
    }

    fprintf(metadata_file, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", bloque_inicial, tamanio);
    fclose(metadata_file);
}

int leer_metadata_archivo(char* nombre, int* bloque_inicial, int* tamanio) {
    char path[256];
    sprintf(path, "%s%s", PATH_METADATA, nombre);

    FILE *metadata_file = fopen(path, "r");
    if (!metadata_file) {
        log_error(logger, "No se pudo abrir el archivo de metadata %s", nombre);
        return 0;
    }

    fscanf(metadata_file, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", bloque_inicial, tamanio);
    fclose(metadata_file);
    return 1;
}

void liberar_bloques(int bloque_inicial, int cantidad) {
    FILE *bitmap_file = fopen(PATH_BITMAP, "rb+");
    if (!bitmap_file) {
        log_error(logger, "No se pudo abrir el archivo bitmap");
        return;
    }

    uint8_t bitmap[BLOCK_COUNT / 8];
    fread(bitmap, sizeof(bitmap), 1, bitmap_file);

    for (int i = 0; i < cantidad; i++) {
        bitmap[(bloque_inicial + i) / 8] &= ~(1 << ((bloque_inicial + i) % 8));
    }

    fseek(bitmap_file, 0, SEEK_SET);
    fwrite(bitmap, sizeof(bitmap), 1, bitmap_file);
    fclose(bitmap_file);
}

void eliminar_metadata_archivo(char* nombre) {
    char path[256];
    sprintf(path, "%s%s", PATH_METADATA, nombre);

    if (remove(path) != 0) {
        log_error(logger, "No se pudo eliminar el archivo de metadata %s", nombre);
    }
}

void actualizar_metadata_archivo(char* nombre, int bloque_inicial, int tamanio) {
    crear_metadata_archivo(nombre, bloque_inicial, tamanio);
}

void escribir_datos_en_bloques(int bloque_inicial, int direccion, char* datos, int cantidad) {
    FILE *bloques_file = fopen(PATH_BLOQUES, "rb+");
    if (!bloques_file) {
        log_error(logger, "No se pudo abrir el archivo de bloques");
        return;
    }

    fseek(bloques_file, bloque_inicial * BLOCK_SIZE + direccion, SEEK_SET);
    fwrite(datos, sizeof(char), cantidad, bloques_file);
    fclose(bloques_file);
}

void leer_datos_de_bloques(int bloque_inicial, int direccion, char* datos, int cantidad) {
    FILE *bloques_file = fopen(PATH_BLOQUES, "rb");
    if (!bloques_file) {
        log_error(logger, "No se pudo abrir el archivo de bloques");
        return;
    }

    fseek(bloques_file, bloque_inicial * BLOCK_SIZE + direccion, SEEK_SET);
    fread(datos, sizeof(char), cantidad, bloques_file);
    fclose(bloques_file);
}

void compactar_fs() {
    log_info(logger, "Iniciando compactación del sistema de archivos");

    // Implementación de compactación: reordenar los bloques ocupados
    // Aquí se debe realizar la lógica para mover los bloques ocupados al inicio del archivo
    // y actualizar las metadata correspondientes.

    sleep(RETRASO_COMPACTACION);
    log_info(logger, "Compactación finalizada");
}

