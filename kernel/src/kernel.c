#include "kernel-estructuras.h"

t_config_kernel *config_kernel; // La declaros como variable global, y la vez instanciar aca!

int main(int argc, char *argv[]) {
    char *config_path = "./kernel.config"; // argv[2]
    
    // Inicializamos logger y cargamos configuracion:
    logger = log_create("kernel.log", "KERNEL", 1, LOG_LEVEL_TRACE);
    config_kernel = cargar_config_kernel(config_path);

    // Generar conexiones con memoria y cpu:
     generar_conexiones_con();

    int socket_servidor = crear_servidor_kernel();

    // Inicializamos modulo:
     //iniciar_modulo_kernel(socket_servidor);
    int seguir = 1;

    //  while (1) {
    //     int socket_cliente = esperar_cliente("Kernel", socket_servidor);

    //     if(socket_cliente != -1) {     
	// 		// pthread_t hilo;
	// 		// t_procesar_conexion *args_hilo = crear_procesar_conexion("IO", socket_cliente);

    //          while(seguir) {
    //             op_code codigo_operacion = recibir_operacion(socket_cliente);

    //             switch(codigo_operacion) {
    //                 case HANDSHAKE:
    //                     recibir_handshake(socket_cliente);
    //                 break;
    //                 case CREAR_INTERFAZ:
    //                     create_interface(socket_cliente);
    //                     seguir = 0;
    //                 break;
    //                 default:
    //                     seguir = 0;
    //             }
    //         }
    //     }
    // }

    t_pcb *pcb = malloc(sizeof(t_pcb));
    pcb->pid = 100;
    pcb->program_counter = 50;
    pcb->registros = malloc(sizeof(t_registro_cpu));

    pcb->registros->AX = 1;
    pcb->registros->BX = 2;
    pcb->registros->CX = 3;
    pcb->registros->DX = 5;
   // pcb->palabra = "HOLA!";

    printf("%i", config_kernel->SOCKET_DISPATCH);
    send_contexto_ejecucion(RECIBIR_PROCESO, config_kernel->SOCKET_DISPATCH, pcb);
    informar_a_memoria();

    return 0;
}