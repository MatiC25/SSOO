#include "cpu-conexiones.h"

atomic_int interrupt_flag = ATOMIC_VAR_INIT(0);

// GENERAMOS CONEXIONES DE CLIENTE A SERVER MEMORIA
void* generar_conexion_a_memoria(void* arg) {
    int md_memoria = 0;
    char* puerto_memoria = string_itoa(config_cpu->PUERTO_MEMORIA);
    char* ip_memoria = config_cpu->IP_MEMORIA;

    if (!ip_memoria){
        log_error(logger, "IP_MEMORIA es NULL");
        //free(puerto_memoria); // Liberar memoria asignada por string_itoa
        return NULL;
    }

    md_memoria = crear_conexion("MEMORIA", ip_memoria, puerto_memoria); // No harcodearlo! Sino leerlo de kernel.config
    if(!md_memoria) {
        return NULL;
    }
    
    generar_handshake(md_memoria, "MEMORIA", ip_memoria, puerto_memoria);
    
    generar_handshake_para_pagina(md_memoria, "MEMORIA", ip_memoria, puerto_memoria);
    
    //log_info(logger, %s, md_memoria);
    config_cpu->SOCKET_MEMORIA = md_memoria;
    return NULL;
}

void generar_handshake_para_pagina(int socket, char *server_name, char *ip, char *puerto){
    int32_t handshake = 1;
    int32_t result;

	op_code cod_op = HANDSHAKEPAGINA; 
	send(socket, &cod_op, sizeof(op_code), 0);


    send(socket, &handshake, sizeof(int32_t), 0);
	recv(socket, &result, sizeof(int32_t), MSG_WAITALL);

    if(!result){ 
        log_info(logger, "Handshake exitoso con %s", server_name);
        config_cpu->TAMANIO_MARCO = recv_pagina(config_cpu->SOCKET_MEMORIA);
    }else {
        log_error(logger, "Error en el handshake con %s", server_name);
        exit(EXIT_FAILURE);
    }

}

int generar_servidor_cpu_dispatch() {
    char* puerto_dispatch = string_itoa(config_cpu->PUERTO_ESCUCHA_DISPATCH); // Convierte un int a una cadena de char
    int md_cpu_ds = iniciar_servidor("DISPATCH", NULL, puerto_dispatch); // Guarda ID del socket
    free(puerto_dispatch);
    while (1){
        int socket_cliente = esperar_cliente("DISPATCH", md_cpu_ds);
        
    if(socket_cliente != -1){
    config_cpu->SOCKET_KERNEL = socket_cliente;
    iniciar_ciclo_de_ejecucion(md_cpu_ds,socket_cliente);
    }
    }
    return md_cpu_ds;
}

int generar_servidor_cpu_interrupt() {
    pthread_t hilo_interrupt;
    char* puerto_interrupt = string_itoa(config_cpu->PUERTO_ESCUCHA_INTERRUPT);
    int md_cpu_it = iniciar_servidor("INTERRUPT",NULL,puerto_interrupt);
    free(puerto_interrupt);

    t_procesar_server* args = malloc(sizeof(t_procesar_server));
    args->server_name = "INTERRUPT";
    args->socket_servidor = md_cpu_it;

    pthread_create(&hilo_interrupt, NULL, server_interrupt, (void*)args);
    pthread_detach(hilo_interrupt); // Usar pthread_detach para no esperar al hilo

    return md_cpu_it;
}

// // CREAMOS SERVIDOR PARA EL CLIENTE KERNEL
void crear_servidores_cpu(int *md_cpu_ds,int *md_cpu_it) {
    *md_cpu_it = generar_servidor_cpu_interrupt();
    *md_cpu_ds = generar_servidor_cpu_dispatch();
    

    if(*md_cpu_ds == -1 || *md_cpu_it == -1) {
        log_error(logger, "No se pudo crear los servidores de escucha");
    }
     
}

void* server_interrupt(void* args) {
    t_procesar_server* arg = (t_procesar_server*) args;
    char* server_namee = arg -> server_name;
    int socket_server = arg -> socket_servidor;
     while (1){
    
        int socket_cliente = esperar_cliente(server_namee, socket_server);
        
    if(socket_cliente != -1){
   
    while (1)
    {
		op_code cod_op = recibir_operacion(socket_cliente);

		switch (cod_op) 
		{
            case HANDSHAKE:
            log_info(logger, "Handshake exitoso con Interrupt");
			case FINQUANTUM:
				atomic_store(&interrupt_flag,1);
			break;
             default:
        log_error(logger, "Operacion desconocida");
        break;
	    }
    }
	
    	}
	}
    
	return NULL;
}