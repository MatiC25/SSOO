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

        log_warning(logger, "Durmiendo %i ...", (tiempo_espera * tiempo_unidad));
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

// void ejecutar_operacion_dialfs(t_interfaz* interfaz){
//     while (1) {
//         int operacion = recibir_entero(interfaz); //hay que añadir un enum con los casos pedidos
//                                                   //cambiar el tipo de "operacion"
//         switch (operacion){
//             case(IO_FS_CREATE):
//                 crear_fs(interfaz);
//                 break;
//             case(IO_FS_DELETE):
//                 borrar_fs(interfaz);
//                 break;
//             case(IO_FS_TRUNCATE):
//                 truncar_fs(interfaz);
//                 break;
//             case(IO_FS_WRITE):
//                 escribir_fs(interfaz);
//                 break;
//             case(IO_FS_READ):
//                 leer_fs(interfaz);
//             default:
//                 log_error(logger, "Operación no soportada en DialFS");
//                 break;
//         }
//     }
// }

// void crear_fs(t_interfaz *interfaz){
//     char *nombre_archivo = recibir_string(interfaz);
//     int bloque_inicial = obtener_bloque_libre();

//     if (bloque_inicial == -1) {
//         log_error(logger, "No hay bloques disponibles");
//         free(nombre_archivo);
//         return;
//     }

//     marcar_bloque_ocupado(bloque_inicial);
//     crear_metadata_archivo(nombre_archivo, bloque_inicial, 0);
//     log_info(logger, "Archivo %s creado en el bloque %d", nombre_archivo, bloque_inicial);

//     free(nombre_archivo);
//     send_respuesta_a_kernel(1, interfaz);
// }

// void borrar_fs(t_interfaz *interfaz){
//     char *nombre_archivo = recibir_string(interfaz);
//     int bloque_inicial, tamanio_archivo;

//     if (!leer_metadata_archivo(nombre_archivo, &bloque_inicial, &tamanio_archivo)){
//         log_error(logger, "No se encontró el archivo %s", nombre_archivo);
//         free(nombre_archivo);
//         return;
//     }

//     liberar_bloques(bloque_inicial, tamanio_archivo / BLOCK_SIZE + 1);
//     eliminar_metadata_archivo(nombre_archivo);  // Eliminar el archivo de metadata
//     log_info(logger, "Archivo %s eliminado", nombre_archivo);

//     free(nombre_archivo);
//     send_respuesta_a_kernel(1, interfaz);
// }

// void truncar_fs(t_interfaz *interfaz){
//     char *nombre_archivo = recibir_string(interfaz);
//     int nuevo_tamanio = recibir_entero(interfaz);

//     int bloque_inicial, tamanio_actual;
//     if (!leer_metadata_archivo(nombre_archivo, &bloque_inicial, &tamanio_actual)){
//         log_error(logger, "No se encontró el archivo %s", nombre_archivo);
//         free(nombre_archivo);
//         return;
//     }

//     if (nuevo_tamanio > tamanio_actual){
//         // Si el nuevo tamaño es mayor, se deben asignar bloques adicionales
//         int bloques_necesarios = (nuevo_tamanio + BLOCK_SIZE - 1) / BLOCK_SIZE;
//         if (!hay_espacio_para_bloques(bloques_necesarios)) {
//             compactar_fs();  // compactar el sistema de archivos si no hay espacio suficiente
//         }
//         asignar_bloques_adicionales(bloque_inicial, nuevo_tamanio);
//     } else {
//         // Si el nuevo tamaño es menor, se deben liberar bloques
//         liberar_bloques(bloque_inicial + nuevo_tamanio / BLOCK_SIZE, (tamanio_actual - nuevo_tamanio) / BLOCK_SIZE);
//     }

//     actualizar_metadata_archivo(nombre_archivo, bloque_inicial, nuevo_tamanio);  // Actualizar la metadata del archivo
//     log_info(logger, "Archivo %s truncado a %d bytes", nombre_archivo, nuevo_tamanio);

//     free(nombre_archivo);
//     send_respuesta_a_kernel(1, interfaz);  // Enviar respuesta a kernel
// }

// void escribir_fs(t_interfaz *interfaz){
//     char *nombre_archivo = recibir_string(interfaz);  // Recibir el nombre del archivo
//     int direccion_fisica = recibir_entero(interfaz);  // Recibir la dirección física donde escribir
//     int bytes_a_escribir = recibir_entero(interfaz);  // Recibir la cantidad de bytes a escribir

//     char *datos = malloc(bytes_a_escribir);  // Reservar memoria para los datos
//     recibir_datos(interfaz, datos, bytes_a_escribir);  // Recibir los datos desde la interfaz

//     int bloque_inicial, tamanio_actual;
//     if (!leer_metadata_archivo(nombre_archivo, &bloque_inicial, &tamanio_actual)){
//         log_error(logger, "No se encontró el archivo %s", nombre_archivo);
//         free(nombre_archivo);
//         free(datos);
//         return;
//     }

//     escribir_datos_en_bloques(bloque_inicial, direccion_fisica, datos, bytes_a_escribir);
//     if (direccion_fisica + bytes_a_escribir > tamanio_actual){
//         actualizar_metadata_archivo(nombre_archivo, bloque_inicial, direccion_fisica + bytes_a_escribir);  // actualizo la metadata si el archivo ha crecido
//     }

//     log_info(logger, "Escribiendo %d bytes en el archivo %s", bytes_a_escribir, nombre_archivo);

//     free(nombre_archivo);
//     free(datos);
//     send_respuesta_a_kernel(1, interfaz);  // Enviar respuesta a kernel
// }

// void leer_fs(t_interfaz *interfaz){
//     char *nombre_archivo = recibir_string(interfaz);
//     int direccion_fisica = recibir_entero(interfaz);
//     int bytes_a_leer = recibir_entero(interfaz); 

//     int bloque_inicial, tamanio_archivo;
//     if (!leer_metadata_archivo(nombre_archivo, &bloque_inicial, &tamanio_archivo)){
//         log_error(logger, "No se encontró el archivo %s", nombre_archivo);
//         free(nombre_archivo);
//         return;
//     }

//     if (direccion_fisica + bytes_a_leer > tamanio_archivo){
//         log_error(logger, "Lectura fuera de los límites del archivo %s", nombre_archivo);
//         free(nombre_archivo);
//         return;
//     }

//     char *datos = malloc(bytes_a_leer);  // reservamos memoria para los datos a leer
//     leer_datos_de_bloques(bloque_inicial, direccion_fisica, datos, bytes_a_leer);  // Leer los datos desde los bloques

//     log_info(logger, "Leyendo %d bytes del archivo %s", bytes_a_leer, nombre_archivo);

//     free(nombre_archivo);
//     free(datos);
//     send_respuesta_a_kernel(1, interfaz);
// }

// void marcar_bloque_ocupado(int bloque) {
//     FILE *bitmap_file = fopen(PATH_BITMAP, "rb+"); //abre el archivo en modo lectura y escritura
//     if (!bitmap_file) {
//         log_error(logger, "No se pudo abrir el archivo bitmap");
//         return;
//     }

//     uint8_t bitmap[BLOCK_COUNT / 8]; //crea un array de 512 bytes
//     fread(bitmap, sizeof(bitmap), 1, bitmap_file); //lee el archivo y lo guarda en bitmap

//     bitmap[bloque / 8] |= (1 << (bloque % 8)); //el bloque se divide por 8 para saber en que byte se encuentra
//                                                //y se hace un OR con un 1 corrido a la derecha la cantidad de veces que indica el modulo
//                                                //esto es para marcar el bit correspondiente al bloque
//     fseek(bitmap_file, 0, SEEK_SET); //mueve el puntero al inicio del archivo
//     fwrite(bitmap, sizeof(bitmap), 1, bitmap_file); //escribe el bitmap en el archivo
//     fclose(bitmap_file);
// }

// int obtener_bloque_libre() {
//     FILE *bitmap_file = fopen(PATH_BITMAP, "rb+");  // Abre el archivo del bitmap en modo lectura y escritura
//     if (!bitmap_file) {
//         log_error(logger, "No se pudo abrir el archivo bitmap");
//         return -1; 
//     }

//     //obtengo el tamaño del archivo
//     fseek(bitmap_file, 0, SEEK_END); //fseek mueve el puntero al lo que diga el 3er parametro, SEEK_END es el final del archivo
//     long bitmap_size = ftell(bitmap_file); //ftell devuelve la posición actual del puntero
//     fseek(bitmap_file, 0, SEEK_SET); //SEEK_SET es el inicio del archivo
//     char *bitmap_data = malloc(bitmap_size);
//     fread(bitmap_data, bitmap_size, 1, bitmap_file); //lee el archivo y lo guarda en bitmap_data

//     t_bitarray *bitmap = bitarray_create_with_mode(bitmap_data, bitmap_size, LSB_FIRST);

//     // Buscar el primer bit que esté en 0, indicando un bloque libre
//     for (int i = 0; i < BLOCK_COUNT; i++) {
//         if (!bitarray_test_bit(bitmap, i)) {
//             bitarray_destroy(bitmap);
//             fclose(bitmap_file);
//             return i;  // retornar el índice del bloque libre
//         }
//     }

//     bitarray_destroy(bitmap);
//     fclose(bitmap_file);
//     return -1;
// }

// void marcar_bloque_ocupado(int bloque) {
//     FILE *bitmap_file = fopen(PATH_BITMAP, "rb+");  // abre el archivo del bitmap en modo lectura y escritura
//     if (!bitmap_file) {
//         log_error(logger, "No se pudo abrir el archivo bitmap");
//         return;
//     }

//     //obtengo el tamaño del archivo
//     fseek(bitmap_file, 0, SEEK_END); //fseek mueve el puntero al lo que diga el 3er parametro, SEEK_END es el final del archivo
//     long bitmap_size = ftell(bitmap_file); //ftell devuelve la posición actual del puntero
//     fseek(bitmap_file, 0, SEEK_SET); //SEEK_SET es el inicio del archivo
//     char *bitmap_data = malloc(bitmap_size);
//     fread(bitmap_data, bitmap_size, 1, bitmap_file); //lee el archivo y lo guarda en bitmap_data

//     t_bitarray *bitmap = bitarray_create_with_mode(bitmap_data, bitmap_size, LSB_FIRST);

//     bitarray_set_bit(bitmap, bloque);

//     fseek(bitmap_file, 0, SEEK_SET);  //va al inicio del archivo del bitmap
//     fwrite(bitmap->bitarray, bitmap_size, 1, bitmap_file);  // escribe el bitmap actualizado en el archivo
//     bitarray_destroy(bitmap);
//     fclose(bitmap_file);
// }

// void crear_metadata_archivo(char* nombre, int bloque_inicial, int tamanio) {
//     char path[256];  // Buffer para la ruta del archivo de metadata(256 es un tamaño arrbitrario, es para asegurar que entre el path completo)
//     sprintf(path, "%s%s", PATH_METADATA, nombre); //sprintf concatena todos los strings en el primer parametro y asi crea la ruta completa

//     FILE *metadata_file = fopen(path, "w");  // abre el archivo de metadata en modo escritura
//     if (!metadata_file) {
//         log_error(logger, "No se pudo crear el archivo de metadata %s", nombre);
//         return; 
//     }

//     // escribe la información de metadata en el archivo
//     fprintf(metadata_file, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", bloque_inicial, tamanio);
//     fclose(metadata_file);
// }

// int leer_metadata_archivo(char* nombre, int* bloque_inicial, int* tamanio) {
//     char path[256];  // Buffer para la ruta del archivo de metadata
//     sprintf(path, "%s%s", PATH_METADATA, nombre); //sprintf concatena todos los strings en el primer parametro y asi crea la ruta completa
//     FILE *metadata_file = fopen(path, "r");
//     if (!metadata_file) {
//         log_error(logger, "No se pudo abrir el archivo de metadata %s", nombre);
//         return 0; 
//     }

//     // Leer la información de metadata del archivo
//     fscanf(metadata_file, "BLOQUE_INICIAL=%d\nTAMANIO_ARCHIVO=%d\n", bloque_inicial, tamanio);//fscanf lee del archivo y guarda en el primer parametro
//     fclose(metadata_file);
//     return 1;
// }

// void liberar_bloques(int bloque_inicial, int cantidad) {
//     FILE *bitmap_file = fopen(PATH_BITMAP, "rb+");  //abre el archivo del bitmap en modo lectura y escritura
//     if (!bitmap_file) {
//         log_error(logger, "No se pudo abrir el archivo bitmap");
//         return;
//     }

//    //obtengo el tamaño del archivo
//     fseek(bitmap_file, 0, SEEK_END); //fseek mueve el puntero al lo que diga el 3er parametro, SEEK_END es el final del archivo
//     long bitmap_size = ftell(bitmap_file); //ftell devuelve la posición actual del puntero
//     fseek(bitmap_file, 0, SEEK_SET); //SEEK_SET es el inicio del archivo
//     char *bitmap_data = malloc(bitmap_size);
//     fread(bitmap_data, bitmap_size, 1, bitmap_file); //lee el archivo y lo guarda en bitmap_data

//     t_bitarray *bitmap = bitarray_create_with_mode(bitmap_data, bitmap_size, LSB_FIRST);

//     // Liberar los bloques especificados en el bitmap
//     for (int i = 0; i < cantidad; i++) {
//         bitarray_clean_bit(bitmap, bloque_inicial + i);
//     }

//     fseek(bitmap_file, 0, SEEK_SET);  // va al inicio del archivo del bitmap
//     fwrite(bitmap->bitarray, bitmap_size, 1, bitmap_file);  // actualiza el archivo bitmap con el bitmap modificado
//     bitarray_destroy(bitmap);
//     fclose(bitmap_file);
// }

// void eliminar_metadata_archivo(char* nombre) {
//     char path[256];  // Buffer para la ruta del archivo de metadata(256 es un tamaño arrbitrario, es para asegurar que entre el path completo)
//     sprintf(path, "%s%s", PATH_METADATA, nombre);  // construye la ruta completa del archivo de metadata

//     // Eliminar el archivo de metadata
//     if (remove(path) != 0) { //remove elimina el archivo :D
//         log_error(logger, "No se pudo eliminar el archivo de metadata %s", nombre);
//     }
// }

// void actualizar_metadata_archivo(char* nombre, int bloque_inicial, int tamanio) {
//     crear_metadata_archivo(nombre, bloque_inicial, tamanio);
// }

// void escribir_datos_en_bloques(int bloque_inicial, int direccion, char* datos, int cantidad) {
//     FILE *bloques_file = fopen(PATH_BLOQUES, "rb+");  // Abre el archivo de bloques en modo lectura y escritura
//     if (!bloques_file) {
//         log_error(logger, "No se pudo abrir el archivo de bloques");
//         return;
//     }

//     fseek(bloques_file, bloque_inicial * BLOCK_SIZE + direccion, SEEK_SET);  // se mueve al inicio del bloque inicial más la dirección
//     fwrite(datos, sizeof(char), cantidad, bloques_file);  // escribe los datos en el archivo de bloques
//     fclose(bloques_file); 
// }

// void leer_datos_de_bloques(int bloque_inicial, int direccion, char* datos, int cantidad) {
//     FILE *bloques_file = fopen(PATH_BLOQUES, "rb");  // Abrir el archivo de bloques en modo lectura
//     if (!bloques_file) {
//         log_error(logger, "No se pudo abrir el archivo de bloques");
//         return;  // Retorna si hay un error al abrir el archivo
//     }

//     fseek(bloques_file, bloque_inicial * BLOCK_SIZE + direccion, SEEK_SET);  // Mover el puntero al inicio del bloque inicial más la dirección
//     fread(datos, sizeof(char), cantidad, bloques_file);  // Leer los datos del archivo de bloques
//     fclose(bloques_file);
// }

// void compactar_fs() {
//     log_info(logger, "Iniciando compactación del sistema de archivos");
//     ////////////////////////////////////////////////////////////////////////////////////
//     ////////////////////////////////////////////////////////////////////////////////////
//     ////////////////////////////////////////////////////////////////////////////////////
//     ////////////////////////////////////////////////////////////////////////////////////
//     //////////////////////////////falta implementar/////////////////////////////////////
//     ////////////////////////////////////////////////////////////////////////////////////
//     ////////////////////////////////////////////////////////////////////////////////////
//     ////////////////////////////////////////////////////////////////////////////////////
//     ////////////////////////////////////////////////////////////////////////////////////
//     sleep(RETRASO_COMPACTACION);
//     log_info(logger, "Compactación finalizada");
// }