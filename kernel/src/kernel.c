#include "kernel-estructuras.h"

t_config_kernel *config_kernel; // La declaros como variable global, y la vez instanciar aca!

int main(int argc, char *argv[]) {
    char *config_path = "./kernel.config"; // argv[2]
    
    // Inicializamos logger y cargamos configuracion:
    logger = log_create("kernel.log", "KERNEL", 1, LOG_LEVEL_TRACE);
    config_kernel = cargar_config_kernel(config_path);

    // Generar conexiones con memoria y cpu:
    generar_conexiones_con();

    // Levantamos servidor:
    //int socket_servidor = crear_servidor_kernel();

    // Inicializamos modulo:
    // iniciar_modulo_kernel(socket_servidor);

    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->pid = 100;
    pcb->program_counter = 50;
    pcb->quantum = 10;
    pcb->registros = malloc(sizeof(t_registro_cpu));
    pcb->estado = NEW;
    pcb->registros->AX = 1;
    pcb->registros->BX = 2;
    pcb->registros->CX = 3;
    pcb->registros->DI = 4;
    pcb->registros->DX = 5;
    pcb->registros->EAX = 5;
    pcb->registros->EBX = 6;
    pcb->registros->ECX = 7;
    pcb->registros->EDX = 8;
    pcb->registros->PC = 9;
    pcb->registros->SI = 10;

    pcb->estado = NEW;

    enviar_proceso_a_cpu(pcb);
    // informar_a_memoria();

    return 0;
}