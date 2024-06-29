#include "consola.h"

int pid_buscado_global;
int esta_pausada;

pthread_mutex_t reanudar_ds;
pthread_mutex_t reanudar_plani;
pthread_mutex_t reanudar_largo;
pthread_mutex_t reanudar_block;


COMMAND comandos[] = {
    {"INICIAR_PROCESO", iniciar_proceso},
    {"PROCESO_ESTADO", proceso_estado},
    {"MULTIPROGRAMACION", multiprogramacion},
    {"EJECUTAR_SCRIPT", ejecutar_script},
    {"FINALIZAR_PROCESO", finalizar_proceso},
    {"DETENER_PLANIFICACION", detener_planificacion},
    {"INICIAR_PLANIFICACION", iniciar_planif},
    {NULL, NULL}
};


void iniciar_consola() {
    char* linea;
    char* operacion;

    inicializar_mutex_consola();

    iniciar_readline();

    while (1) {
        linea = readline("C-Comenta: ");

        if (!linea)
            break;

        operacion = eliminar_espacios(linea);

        if (operacion != NULL) {
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

    while (linea[i] && isspace(linea[i]))
        i++;

    palabra = linea + i;

    while (linea[i] && !isspace(linea[i]))
        i++;

    if (linea[i])
        linea[i++] = '\0';

    comando = encontrar_comando(palabra);

    if (!comando) {
        printf("Comando desconocido: %s\n", palabra);
        return -1;
    }

    while (linea[i] && isspace(linea[i]))
        i++;

    palabra = linea + i;

    ((comando->funcion)(palabra));

    return 1;
}


COMMAND* encontrar_comando(char* nombre) {
    for (int i = 0; comandos[i].nombre; i++) {
        if (strcmp(nombre, comandos[i].nombre) == 0)
            return &comandos[i];
    }

    return NULL;
}


void iniciar_readline() {
    rl_readline_name = "C-Comenta";
    rl_attempted_completion_function = completar_CComenta;
}


char** completar_CComenta(const char* texto, int inicio, int fin) {
    char** matches = NULL;

    if (inicio == 0)
        matches = rl_completion_matches(texto, generador_de_comandos);

    return matches;
}


char* generador_de_comandos(const char* texto, int estado) {
    static int lista_index, len;
    char* nombre;

    if (!estado) {
        lista_index = 0;
        len = strlen(texto);
    }

    while ((nombre = comandos[lista_index].nombre)) {
        lista_index++;

        if (strncmp(nombre, texto, len) == 0)
            return strdup(nombre);
    }

    return NULL;
}


void* iniciar_proceso(void* args) {
    char *path = (char*) args; // Path del archivo a ejecutar

    creacion_proceso(path); // Invocamos a la función que crea el proceso
    return NULL;
}


void* proceso_estado(void* args) {
    printf("\nListando procesos por estado:\n");

    pthread_mutex_lock(&mutex_estado_new);
    imprimir_procesos_en_cola("NEW", cola_new);
    pthread_mutex_unlock(&mutex_estado_new);

    pthread_mutex_lock(&mutex_estado_ready);
    imprimir_procesos_en_cola("READY", cola_ready);
    pthread_mutex_unlock(&mutex_estado_ready);

    imprimir_proceso_exec();

    pthread_mutex_lock(&mutex_estado_block);
    imprimir_procesos_en_cola("BLOCK", cola_block);
    pthread_mutex_unlock(&mutex_estado_block);

    pthread_mutex_lock(&mutex_exit);
    imprimir_procesos_en_cola("EXIT", cola_exit);
    pthread_mutex_unlock(&mutex_exit);

    return NULL;
}


void imprimir_procesos_en_cola(char* estado, t_list* cola) {
    int size_estados_proceso = list_size(cola);

    log_info("Estado %s:\n", estado);

    for (int i = 0; i < size_estados_proceso; i++) {
        t_pcb* pcb = list_get(cola, i);
        log_info(logger, "Proceso/s en %s: %d\n", estado, pcb->pid);
    }
}


void imprimir_proceso_exec() {
    pthread_mutex_lock(&mutex_estado_exec);
    if(proceso_en_exec->estado == EXEC) {
        log_info(logger, "Estado EXEC:\n");
        if (proceso_en_exec) {
            log_info(logger, "Proceso en EXEC: %d\n", proceso_en_exec->pid);
        } else {
            log_info(logger, "No hay proceso en ejecución.\n");
        }
    }
    pthread_mutex_unlock(&mutex_estado_exec);
}


void* multiprogramacion(void* args) {
    char *multiprogramacion = (char*) args;
    int valor_multiprogramacion = atoi(multiprogramacion);

    for (int i = 0; i < valor_multiprogramacion; i++)
        sem_post(&sem_multiprogramacion);
    return NULL;
}


void* ejecutar_script(void* args) {
    char *script_path = (char*) args;
    char *path_inicial = "/home/utnso/c-comenta-pruebas-main/preliminares/";

    // Calcular el tamaño necesario para path_nuevo
    size_t len_path_nuevo = strlen(path_inicial) + strlen(script_path) + 1;

    // Asignar memoria para path_nuevo
    char *path_nuevo = malloc(len_path_nuevo);
    if (path_nuevo == NULL) {
        log_error(logger, "Error al asignar memoria para path_nuevo");
        return NULL;
    }

    // Copiar path_inicial a path_nuevo
    strcpy(path_nuevo, path_inicial);

    // Concatenar script_path a path_nuevo
    strcat(path_nuevo, script_path);

    FILE* archivo_script = fopen(path_nuevo, "r");
    if (!archivo_script) {
        log_error(logger, "¡Archivo de script erroneo!");
        free(path_nuevo);
        return NULL;
    }

    char comando[MAX_COMMAND_LETTERS];
    while (fgets(comando, sizeof(comando), archivo_script)) {
        comando[strcspn(comando, CENTINELA)] = '\0';
        ejecutar_comando(comando);
    }

    fclose(archivo_script);
    free(path_nuevo);
    return NULL;
}


void* finalizar_proceso(void* pid) {
    int pid_buscado = atoi((char*)pid);

    pthread_mutex_lock(&mutex_estado_exec);
    if (proceso_en_exec != NULL && pid_buscado == proceso_en_exec->pid && proceso_en_exec->estado == EXITT) {
        finalizar_por_invalidacion(proceso_en_exec, "INTERRUPTED_BY_USER");
        log_info(logger, "Proceso a Finalizar encontrado en EXEC");
        pthread_mutex_unlock(&mutex_estado_exec);
    } else {
        pthread_mutex_unlock(&mutex_estado_exec);

        pthread_mutex_lock(&mutex_estado_block);
        t_pcb* pcb = pcb_encontrado(cola_block, pid_buscado);
        if (pcb != NULL) {
            finalizar_por_invalidacion(pcb, "INTERRUPTED_BY_USER");
            log_info(logger, "Proceso a Finalizar encontrado en BLOCK");
            pthread_mutex_unlock(&mutex_estado_block);
        } else {
            pthread_mutex_unlock(&mutex_estado_block);

            pthread_mutex_lock(&mutex_estado_ready);
            pcb = pcb_encontrado(cola_ready, pid_buscado);
            if (pcb != NULL) {
                finalizar_por_invalidacion(pcb, "INTERRUPTED_BY_USER");
                log_info(logger, "Proceso a Finalizar encontrado en READY");
                pthread_mutex_unlock(&mutex_estado_ready);
            } else {
                pthread_mutex_unlock(&mutex_estado_ready);

                pthread_mutex_lock(&mutex_cola_priori_vrr);
                pcb = pcb_encontrado(cola_prima_VRR, pid_buscado);
                if (pcb != NULL) {
                    finalizar_por_invalidacion(pcb, "INTERRUPTED_BY_USER");
                    log_info(logger, "Proceso a Finalizar encontrado en READY_VRR");
                    pthread_mutex_unlock(&mutex_cola_priori_vrr);
                } else {
                    pthread_mutex_unlock(&mutex_cola_priori_vrr);
                    log_error(logger, "¡No existe el PID indicado! Ingrese nuevamente ...");
                }
            }
        }
    }
    return NULL;
}


t_pcb* pcb_encontrado(t_list* cola_a_buscar_pid, int pid_buscado) {

    pid_buscado_global = pid_buscado;
    t_pcb* resultado = list_find(cola_a_buscar_pid, es_el_proceso_buscado);
    return resultado;
}


bool existe_proceso_con_pid_ingresado(t_list* cola_a_buscar_pid, int pid_buscado) {
    pid_buscado_global = pid_buscado;
    return list_any_satisfy(cola_a_buscar_pid, es_el_proceso_buscado);
}


bool es_el_proceso_buscado(void* elemento) {
    t_pcb* pcb = (t_pcb*)elemento;
    return pid_buscado_global == pcb->pid;
}


// /*Detener planificacion: Este mensaje se encargara de pausar la planificacion de corto y largo plazo.
// El proceso que se encuentra en ejecucion NO es desalojado, pero una vez que salga de EXEC se va a 
// pausar el manejo de su motivo de desalojo. De la misma forma, los procesos bloqueados van a pausar
// su transicion a la cola de Ready.
// Nomenclatura: DETENER_PLANIFICACION*/

void *detener_planificacion(void* args) {
    if(esta_pausada == 1) {
        log_warning(logger, "La consola ya se encuentra detenida...");
    }
    else {
        pthread_mutex_lock(&reanudar_plani);
        pthread_mutex_lock(&reanudar_largo);
        pthread_mutex_lock(&reanudar_ds);
        pthread_mutex_lock(&reanudar_block);
        esta_pausada = 1;
        log_info(logger, "Planificacion pausada!");
    }
}
// Iniciar planificacion: Este mensaje se encargara de retomar (en caso que se encuentre pausada) 
// la planificacion de corto y largo plazo. En caso que la planificacion no se encuentre pausada, 
// se debe ignorar el mensaje.
// Nomenclatura: INICIAR_PLANIFICACION


void *iniciar_planif(void* args){
    if(esta_pausada == 1){
        pthread_mutex_unlock(&reanudar_plani);
        pthread_mutex_unlock(&reanudar_largo);
        pthread_mutex_unlock(&reanudar_ds);
        pthread_mutex_unlock(&reanudar_block);
        esta_pausada = 0;
        log_info(logger, "Planificacion reanudada!");
    } else {
        log_info(logger, "La planificación ya está en marcha, ignorando comando ...");
    }
}


void inicializar_mutex_consola() {
    pthread_mutex_init(&reanudar_plani, NULL);
    pthread_mutex_init(&reanudar_largo, NULL);
    pthread_mutex_init(&reanudar_ds, NULL);
    pthread_mutex_init(&reanudar_block, NULL);
}

