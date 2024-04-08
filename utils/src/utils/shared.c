#include "shared.h"

bool tiene_todas_las_configuraciones(t_config *config, char *configs[])
{
    for (size_t i = 0; configs[i] != NULL; i++)
    {
        if (!config_has_property(config, configs[i]))
            return false;
    }

    return true;
}

bool es_esta_palabra(char *palabra, char *palabraNecesaria)
{ // Funcion: compara dos palabras y devuelve true si son iguales
    return strcmp(palabra, palabraNecesaria) == 0;
}

bool tiene_algun_algoritmo_de_reemplazo(char *palabra)
{ // Funcion: devuelve true si la palabra es CLOCK o LRU
    if (es_esta_palabra(palabra, "FIFO"))
        return true;
    else
    {
        if (es_esta_palabra(palabra, "LRU"))
            return true;
    }

    return false;
}

bool tiene_algun_algoritmo_de_planificacion(char *palabra)
{ // Funcion: devuelve true si la palabra es FIFO, RR o PRIORIDADES
    if (es_esta_palabra(palabra, "FIFO"))
        return true;
    else
    {
        if (es_esta_palabra(palabra, "RR"))
            return true;
        else
        {
            if (es_esta_palabra(palabra, "PRIORIDADES"))
                return true;
        }
    }

    return false;
}

void liberar_espacios_de_memoria(void **vector)
{ // Funcion: libera todos los espacios de memoria de un array de punteros asignados dinamicamente
    for (size_t i = 0; vector[i] != NULL; i++)
    {
        free(vector[i]);
    }

    free(vector);
} // Comentario: tipo de dato (cualquiera)** nombre de la variable = array de punteros

void copiar_valor(char **destino, char *valor)
{ // Funcion: copia el valor de una variable a otra
    *destino = malloc(strlen(valor) + 1);
    strcpy(*destino, valor);
}

void crear_vector_dinamico_char(char ***vector, char *informacion[])
{ // Funcion: crea un vector dinamico de char
    int cantidad_de_elementos = string_array_size(informacion);
    *vector = (char **)malloc(sizeof(char *) * (cantidad_de_elementos + 1));
    (*vector)[cantidad_de_elementos] = NULL;

    for (size_t i = 0; informacion[i] != NULL; i++)
        (*vector)[i] = strdup(informacion[i]);
}

void crear_vector_dinamico_int(int **vector, char *informacion[])
{
    int cantidad_de_elementos = string_array_size(informacion);
    *vector = (int *)malloc(sizeof(int *) * (cantidad_de_elementos + 1));

    for (size_t i = 0; informacion[i] != NULL; i++)
        (*vector)[i] = atoi(informacion[i]);
}

void recorrer_vector_char(char *vector[])
{
    for (int i = 0; vector[i] != NULL; i++)
        printf("%s\n", vector[i]);
}
