#include "consola.h"

COMMAND comandos[] = {
    {"INICIAR_PROCESO", iniciar_proceso},
    /*{"FINALIZAR_PROCESO", finalizar_proceso},
    {"DETENER_PLANIFICACION", detener_planificacion},
    {"INICIAR_PLANIFICACION", iniciar_planificacion},
    {"MULTIPROGRAMACION", multiprogramacion},
    {"PROCESO_ESTADO", proceso_estado},
    {"HELP", help},*/
    {NULL, NULL}
};

void iniciar_consola() {
    char* linea;
    char* operacion;

    iniciar_readline();

    while(1) {
        linea = readline("C-Comenta: ");

        if(!linea)
            break;

        operacion = eliminar_espacios(linea);

        if(operacion != NULL) {
            add_history(linea);
            ejecutar_comando(linea);
        }
           
        free(linea);
    }
}


int ejecutar_comando(char* linea) {
    int i = 0;
    char* palabra;
    COMMAND* comando;

    while(linea[i] && isspace(linea[i]))
        i++;
    
    palabra = linea + i;

    while(linea[i] && !isspace(linea[i]))
        i++;
    
    if(linea[i])
        linea[i++] = '\0';

    comando = encontrar_comando(palabra);

    if(!comando) {
        printf("Comando desconocido: %s\n", palabra);

        return -1;
    }

    while(linea[i] && isspace(linea[i]))
        i++;

    palabra = linea + i;

    ((comando->funcion) (palabra));

    return 1;
}

COMMAND* encontrar_comando(char* nombre) {
    for(int i = 0; comandos[i].nombre; i++) {
        if(strcmp(nombre, comandos[i].nombre) == 0)
            return &comandos[i];
    }

    return ((COMMAND*) NULL);
}

void iniciar_readline() {
    rl_readline_name = "C-Comenta";
    rl_attempted_completion_function = completar_CComenta;
}

char** completar_CComenta(const char* texto, int inicio, int fin) {
    char** matches = NULL;

    if(inicio == 0)
        matches = rl_completion_matches(texto, generador_de_comandos);

    return (matches);
}

char* generador_de_comandos(const char* texto, int estado) {
    static int lista_index, len;
    char* nombre;

    if(!estado) {
        lista_index = 0;
        len = strlen(texto);
    }

    while((nombre = comandos[lista_index].nombre)) {
        lista_index++;

        if(strncmp(nombre, texto, len) == 0)
            return strdup(nombre);
    }

    return ((char*) NULL);
}

void* iniciar_proceso(void* args) {
    char *path_proceso = (char *) args;
    
}