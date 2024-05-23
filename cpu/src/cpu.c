#include "cpu.h"

t_pcb_cpu* pcb_cpu
t_config_cpu *config_cpu; // La declaro como variable global, y la vez instanciar aca!
int hay_que_seguir = 1;

int main(void) {
//Creando logger
    int md_cpu_ds = 0;
    int md_cpu_it = 0;
    logger = log_create("CPUlog.log", "CPU", 1, LOG_LEVEL_INFO);

    if (logger == NULL) {
		perror("No se puedo encontrar el archivo");
		return EXIT_FAILURE;
	}
    
    inicializar_config(); // Inicializo la variable global config_kernel! -> No se si es la mejor forma de hacerlo!
    generar_conexion_a_memoria();
    crear_servidores(&md_cpu_ds, &md_cpu_it);
    iniciar_ciclo_de_ejecucion(md_cpu_ds);

    return EXIT_SUCCESS;
}


