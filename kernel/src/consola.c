#include "consola.h"

COMMAND comandos[] = {
    {"INICIAR_PROCESO", iniciar_proceso},
    {"PROCESO_ESTADO", proceso_estado},
    {"MULTIPROGRAMACION", multiprogramacion},
    {"EJECUTAR_SCRIPT", ejecutar_script},
    {"FINALIZAR_PROCESO", finalizar_proceso},
    /*{"DETENER_PLANIFICACION", detener_planificacion},
    {"INICIAR_PLANIFICACION", iniciar_planificacion},
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
    //estados_procesos[2] = cola_exec;
    estados_procesos[3] = cola_block;
    //estados_procesos[4] = cola_exit;
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

void *ejecutar_script(void *args) {
    char* script_path = (char*) args;

    FILE* archivo_script = fopen(script_path, "r");
    if(!archivo_script) {
        log_error(logger, "¡Archivo de script erroneo!");
    }

    char comando [MAX_COMMAND_LETTERS];
    while(fgets(comando, sizeof(MAX_COMMAND_LETTERS), archivo_script)) {
        comando[strcspn(comando, CENTINELA)] = '\0';
        ejecutar_comando(comando);
        log_info(logger, "COMANDO LEIDO: %s", comando);
    }
    
    fclose(archivo_script);
}

/*Finalizar proceso: Se encargara de finalizar un proceso que se encuentre dentro del sistema. 
Este mensaje se encargara de realizar las mismas operaciones como si el proceso llegara a EXIT 
por sus caminos habituales (debera liberar recursos, archivos y memoria).
Nomenclatura: FINALIZAR_PROCESO [PID]*/

void *finalizar_proceso(void *pid) {
    int pid_buscado = *(int*) pid;

    pthread_mutex_lock(&mutex_estado_exec);
    if (proceso_en_exec != NULL && pid_buscado == proceso_en_exec->pid) {
        finalizar_por_invalidacion(proceso_en_exec, "INTERRUPTED_BY_USER");
        pthread_mutex_unlock(&mutex_estado_exec);
    } else {
        pthread_mutex_unlock(&mutex_estado_exec);

        pthread_mutex_lock(&mutex_estado_block);
        if (existe_proceso_con_pid_ingresado(cola_block, pid_buscado)) {
            finalizar_por_invalidacion(pcb_encontrado(cola_block, pid_buscado), "INTERRUPTED_BY_USER");
            pthread_mutex_unlock(&mutex_estado_block);
        } else {
            pthread_mutex_unlock(&mutex_estado_block);

            pthread_mutex_lock(&mutex_estado_ready);
            if (existe_proceso_con_pid_ingresado(cola_ready, pid_buscado)) {
                finalizar_por_invalidacion(pcb_encontrado(cola_ready, pid_buscado), "INTERRUPTED_BY_USER");
                pthread_mutex_unlock(&mutex_estado_ready);
            } else {
                pthread_mutex_unlock(&mutex_estado_ready);

                pthread_mutex_lock(&mutex_cola_priori_vrr);
                if (existe_proceso_con_pid_ingresado(cola_prima_VRR, pid_buscado)) {
                    finalizar_por_invalidacion(pcb_encontrado(cola_prima_VRR, pid_buscado), "INTERRUPTED_BY_USER");
                    pthread_mutex_unlock(&mutex_cola_priori_vrr);
                } else {
                    pthread_mutex_unlock(&mutex_cola_priori_vrr);
                    log_error(logger, "¡No existe el PID indicado! Ingrese nuevamente ...");
                }
            }
        }
    }
}

t_pcb* pcb_encontrado(t_list* cola_a_buscar_pid, int pid_buscado) {
    if (existe_proceso_con_pid_ingresado(cola_a_buscar_pid, pid_buscado)) {
        return list_find(cola_a_buscar_pid, es_el_proceso_buscado, &pid_buscado);
    } else {
        return NULL;
    }
}

bool existe_proceso_con_pid_ingresado(t_list* cola_a_buscar_pid, int pid_buscado) {
    return list_any_satisfy(cola_a_buscar_pid, es_el_proceso_buscado, &pid_buscado);
}

// Función auxiliar para comparar el PID
bool es_el_proceso_buscado(void* elemento, void* aux) {
    t_pcb* pcb = (t_pcb*)elemento;
    int *pid_aux = *(int*)aux;
    return pid_aux == pcb->pid;
}


// /*Detener planificacion: Este mensaje se encargara de pausar la planificacion de corto y largo plazo.
// El proceso que se encuentra en ejecucion NO es desalojado, pero una vez que salga de EXEC se va a 
// pausar el manejo de su motivo de desalojo. De la misma forma, los procesos bloqueados van a pausar
// su transicion a la cola de Ready.
// Nomenclatura: DETENER_PLANIFICACION*/
// // void *detener_planificacion(void* args) {
    
// }

// Iniciar planificacion: Este mensaje se encargara de retomar (en caso que se encuentre pausada) 
// la planificacion de corto y largo plazo. En caso que la planificacion no se encuentre pausada, 
// se debe ignorar el mensaje.
// Nomenclatura: INICIAR_PLANIFICACION


// void *iniciar_planificacion(void* args){
//     if(la_plani_esta_pausafa){ //tampoco se como ver esta condicion
//         pthread_mutex_unlock(&planificacion_cortoplazo); //leito, fijate que semaforo es
//     } else {
//         log_info(logger, "La planificación ya está en marcha");
//     }
// }

