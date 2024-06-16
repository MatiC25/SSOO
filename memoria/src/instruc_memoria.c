#include "instruc_memoria.h"

char* crear_path_instrucciones(char* archivo_path){

    char *path = string_new();
    string_append(&path, config_memoria->path_instrucciones);
    string_append(&path, "/");
    string_append(&path, archivo_path);

    if(string_contains(path ,"./")){
        char *buffer = malloc(100*sizeof(char));
        getcwd(buffer, 100);
        string_append(&buffer, "/");
        path = string_replace(path, "./", buffer);
    }
    else if(string_contains(path, "~/")){
		path = string_replace(path, "~/", "/home/utnso/");
	}
    return path;
}
// Obtenemos las instrucciones de los archivos de pseudocódigo

void leer_archivoPseudo(int socket_kernel){

    char* archivo_path;
    int pid;

    recv_archi_pid(socket_kernel, &archivo_path, &pid);

    char* path = crear_path_instrucciones(archivo_path);

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
    while(feof(archivo) == 0 ){ //&& program_counter != -1

        //Accedemos a cada linea del archivo
        cadena = malloc(500);
        char* dato_cadena = fgets(cadena, 500, archivo); // Fgets lee cada linea

        if(dato_cadena == NULL){
            log_error(logger, "Archivo vacío, linea 33");
            break;
        }

        if(string_contains(cadena, "\n")){
		    char** lista_de_cadenas = string_split(cadena, "\n");

		    cadena = string_array_pop(lista_de_cadenas);


	        while(lista_de_cadenas != NULL && strcmp(cadena,"") == 0){
			    cadena = string_array_pop(lista_de_cadenas);
		    }

		    string_array_destroy(lista_de_cadenas);
	    }

        t_instruccion *ptr_inst = malloc(sizeof(t_instruccion));


        ptr_inst->parametro1  =  NULL;
        ptr_inst->parametro2  =  NULL;
        ptr_inst->parametro3  =  NULL;
        ptr_inst->parametro4  =  NULL;
        ptr_inst->parametro5  =  NULL;

        char *token = strtok(cadena," "); //obtenesmo el opcode(esta separado por un espacio)
        ptr_inst -> opcode = token;
        ptr_inst -> long_opcode = strlen(ptr_inst -> opcode) + 1;

        //obtengo los parametros(tambien estan separados por un espacSio)
        token = strtok(NULL," ");
        ptr_inst -> parametro1 = token;
        //me aseguro que no siga sacando si no hay mas parametros
        if (token != NULL){
            token = strtok(NULL," ");
            ptr_inst -> parametro2 = token;

            if (token != NULL){
                token = strtok(NULL," ");
                ptr_inst -> parametro3 = token;

                if (token != NULL){
                    token = strtok(NULL," ");

                    ptr_inst -> parametro4 = token;
                    if (token != NULL){
                        token = strtok(NULL," ");
                        ptr_inst -> parametro5 = token;
                   }
                }
            }
        }
        if(ptr_inst -> parametro1 != NULL){
            ptr_inst -> long_par1 = strlen(ptr_inst -> parametro1)+1;
        } else {
            ptr_inst -> long_par1 = 0;
        }
        if(ptr_inst -> parametro2 != NULL){
            ptr_inst -> long_par2 = strlen(ptr_inst -> parametro2)+1;
        } else {
            ptr_inst -> long_par2 = 0;
        }
        if(ptr_inst -> parametro3 != NULL){
            ptr_inst -> long_par3 = strlen(ptr_inst -> parametro3)+1;
        } else {
            ptr_inst -> long_par3 = 0;
        }
        if(ptr_inst -> parametro4 != NULL){
            ptr_inst -> long_par4 = strlen(ptr_inst -> parametro4)+1;
        } else {
            ptr_inst -> long_par4 = 0;
        }
        if(ptr_inst -> parametro5 != NULL){
            ptr_inst -> long_par5 = strlen(ptr_inst -> parametro5)+1;
        } else {
            ptr_inst -> long_par5 = 0;
        }

        list_add(lista_de_instrucciones,ptr_inst);

    }
    //añadimos a un diccionario para usarlo mas tarde para enviar la instruccion a cpu
    dictionary_put(lista_instrucciones_porPID, string_itoa(pid), lista_de_instrucciones);
    
    free(path);
    fclose(archivo);
}

void enviar_instruccion_a_cpu(int socket_cpu, int retardo_de_respuesta){

    int pid;
    int program_counter;

    recibir_program_counter(socket_cpu, &pid, &program_counter);//lo hice gede asi no nos olvidamos
  

    t_list* lista_de_instrucciones = dictionary_get(lista_instrucciones_porPID, string_itoa(pid));

    if(lista_de_instrucciones == NULL){
        log_error(logger, "no se hayo la lista de instrucciones en el psedocodigo");
    }

    //consigo la instruccion actual. como el PC indica la siguiente instruccion a ejecutar, le resto 1
    t_instruccion *instrucciones = list_get(lista_de_instrucciones, program_counter-1);

    //creo el paquete con la instruccion y serializo
    t_paquete *paquete_de_instrucciones = crear_paquete(INSTRUCCION);
    agregar_a_paquete(paquete_de_instrucciones, instrucciones -> opcode, instrucciones -> long_opcode);
    agregar_a_paquete(paquete_de_instrucciones, instrucciones -> parametro1, instrucciones -> long_par1);
    agregar_a_paquete(paquete_de_instrucciones, instrucciones -> parametro2, instrucciones -> long_par2);
    agregar_a_paquete(paquete_de_instrucciones, instrucciones -> parametro3, instrucciones -> long_par3);
    agregar_a_paquete(paquete_de_instrucciones, instrucciones -> parametro4, instrucciones -> long_par4);
    agregar_a_paquete(paquete_de_instrucciones, instrucciones -> parametro5, instrucciones -> long_par5);
    //agrego el retardo pedido por la consigna
    retardo_pedido(retardo_de_respuesta);

    enviar_paquete(paquete_de_instrucciones, socket_cpu);
    eliminar_paquete(paquete_de_instrucciones);
}


