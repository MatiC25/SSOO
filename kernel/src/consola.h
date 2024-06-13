#ifndef CONSOLA_H_
#define CONSOLA_H_

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <utils/shared.h>


#include "planificador.h"

typedef struct {
    char* nombre;
    void* (*funcion)(void*); // Firma de las funciones!
} COMMAND;

// Consola:
void iniciar_consola();

// Readline:
void iniciar_readline();
char** completar_CComenta(const char* texto, int inicio, int fin);
char* generador_de_comandos(const char* texto, int estado);

// Ejecución de comandos:
int ejecutar_comando(char* linea);
COMMAND* encontrar_comando(char* nombre);

// Comandos
void* iniciar_proceso(void* args);

#endif