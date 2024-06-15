#include "cpu.h"

//t_pcb_cpu* pcb_cpu;
t_config_cpu *config_cpu; // La declaro como variable global, y la vez instanciar aca!
int hay_que_seguir = 1;


int main(void) {
//Creando logger
    int md_cpu_ds = 0;
    int md_cpu_it = 0;
    logger = log_create("CPUlog.log", "CPU", 1, LOG_LEVEL_INFO);

    if (!logger) {
		perror("No se puedo encontrar el archivo");
		return EXIT_FAILURE;
	}
    
    config_cpu = inicializar_config(); // Inicializo la variable global config_kernel! -> No se si es la mejor forma de hacerlo!
    cargar_configuraciones(config_cpu);

    

    pthread_t hilo_memoria;
    pthread_create(&hilo_memoria, NULL, generar_conexion_a_memoria, NULL);
    pthread_join(hilo_memoria,NULL);

    // crear_servidores_cpu(&md_cpu_ds, &md_cpu_it);
    

    return EXIT_SUCCESS;
}