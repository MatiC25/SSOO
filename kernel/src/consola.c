#include "consola.h"

COMMAND comandos[] = {
    {"INICIAR_PROCESO", iniciar_proceso},
    {"PROCESO_ESTADO", proceso_estado},
    /*{"FINALIZAR_PROCESO", finalizar_proceso},
    {"DETENER_PLANIFICACION", detener_planificacion},
    {"INICIAR_PLANIFICACION", iniciar_planificacion},
    {"MULTIPROGRAMACION", multiprogramacion},
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
    char *path = (char*) args; // Path del archivo a ejecutar

    creacion_proceso(path); // Invocamos a la función que crea el proces
        return NULL;

}


void *proceso_estado(void *args) {
    int size_estados_proceso;
    t_list *estados_procesos[5];

    agregar_procesos_a_lista(estados_procesos);

    for(int i = 0; i < 5; i++) {
        t_list *estado_aux = estados_procesos[i];
        size_estados_proceso = list_size(estado_aux);

        for(int j = 0; j < size_estados_proceso; j++) {
            t_pcb *pcb = list_get(estado_aux, j);
            printf("Proceso %d: %s\n", pcb->pid, estado_proceso(i)); // Imprimimos el estado del proceso
        }
    }
        return NULL;

} 

void agregar_procesos_a_lista(t_list *estados_procesos[]) {
    estados_procesos[0] = cola_new;
    estados_procesos[1] = cola_ready;
    // estados_procesos[2] = cola_exec;
    // estados_procesos[3] = cola_block;
    // estados_procesos[4] = cola_exit;
}

char *estado_proceso(int estado) {
    switch(estado) {
        case 0:
            return "NEW";
        case 1:
            return "READY";
        case 2:
            return "EXEC";
        case 3:
            return "BLOCK";
        case 4:
            return "EXIT";
    }
        return NULL;

}

void *multiprogramacion(void *args) {
    char *multiprogramacion = (char*) args;
    int valor_multiprogramacion = atoi(multiprogramacion);

    for(int i = 0; i < valor_multiprogramacion; i++)
        sem_post(&sem_multiprogramacion);
    return NULL;
}