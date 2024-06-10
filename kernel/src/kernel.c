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
    int socket_servidor = crear_servidor_kernel();

    // Inicializamos modulo:
    // iniciar_modulo_kernel(socket_servidor);

    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->pid = 1;
    pcb->program_counter = 0;
    pcb->quantum = 0;
    pcb->registros = malloc(sizeof(t_registro_cpu));
    pcb->estado = NEW;
    pcb->registros->AX = 0;
    pcb->registros->BX = 0;
    pcb->registros->CX = 0;
    pcb->registros->DI = 0;
    pcb->registros->DX = 0;
    pcb->registros->EAX = 0;
    pcb->registros->EBX = 0;
    pcb->registros->ECX = 0;
    pcb->registros->EDX = 0;
    pcb->registros->PC = 0;
    pcb->registros->SI = 0;

    pcb->estado = NEW;
    pcb->program_counter = 0;

    enviar_proceso_a_cpu(pcb);
    informar_a_memoria();

    return 0;
}