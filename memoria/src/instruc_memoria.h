#ifndef INSTRUC_MEMORIA_H
#define INSTRUC_MEMORIA_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <utils/socket.h>
#include <utils/shared.h>
#include <utils/estructuras_compartidas.h>
#include <utils/protocolo.h>
#include <pthread.h>
#include <utils/logger.h>
typedef struct {  
    
    char opcode;
    char parametro1;
    char parametro2;
    char parametro3;
    char parametro4;
    char parametro5;
    int long_opcode;
    int long_par1;
    int long_par2;
    int long_par3;
    int long_par4;
    int long_par5;
} t_instruccion;

t_dictionary* lista_instrucciones_porPID;

extern char* path_proceso;
extern t_dictionary* instrucciones_PorProcesos;


void leer_archivoPseudo(int socket_kernel);
char* crear_path_instrucciones(char* path_proceso, char* archivo_path);
void enviar_instruccion_a_cpu(int socket_cpu, &pid, &program_counter);
void recibir_program_counter(int socket_cpu,int *pid,int *program_counter);


#endif // INSTRUC_MEMORIA_H