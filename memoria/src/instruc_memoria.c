#include "instruc_memoria.h"

// Obtenemos las instrucciones de los archivos de pseudocódigo

void leer_archivoPseudo(int socket_cpu){

char* archivo_path;
int pid;

recv_archi_pid(socket_cpu, &archivo_path, &pid);

char* path = crear_path_instrucciones(&path_proceso, archivo_path);

//Abrimos archivo_path para leer broOOOOoooOoder
FILE *archivo = fopen(path, "r");

	//Comprobar si el archivo existe
    if(archivo == NULL){
		log_error(logger, "Error en la apertura del archivo: Error");
		free(path);
		return;
	}


char* cadena;
t_list* lista_de_instrucciones = list_create();

// Estamos leyendo el archivo, y pasandolo a una lista
while(feof(archivo) == 0){

    //Accedemos a cada linea del archivo
    cadena = malloc(500);
    char* dato_cadena = fgets(cadena, 500, archivo); // Fgets lee cada linea

    if(dato_cadena == NULL){
        log_error(logger, "Archivo vacío, linea 34");
        break;
    }

    if(string_contains(cadena, "\n")){
		char** lista_de_cadenas = string_split(cadena, "\n");

		cadena = string_array_pop(lista_de_cadenas);


		while(strcmp(cadena, "") == 0){
			cadena = string_array_pop(lista_de_cadenas);
		}

		string_array_destroy(lista_de_cadenas);
	}


}










}


char* crear_path_instrucciones(char* path_proceso, char* archivo_path){

    char *path = string_new();
    string_append(&path, path_proceso);
    string_append(&path, "/");
    string_append(&path, archivo_path);

    if(string_contains(path ,"./")){
        char *buffer = malloc(100*sizeof(char));
        getcwd(buffer, 100);
        string_append(&buffer, "/");
        path = string_replace(&path, "./", buffer);
    }
    else if(string_contains(path, "~/")){
		path = string_replace(path, "~/", "/home/utnso/");
	}

    return path;
}