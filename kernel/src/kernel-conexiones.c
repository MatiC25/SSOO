void generar_conexiones_con_cpu(void) {
    int md_dispatch_cpu = 0;
    int md_interrupt_cpu = 0;

    char* puerto_dispatch = string_itoa(config_kernel->PUERTO_CPU_DISPATCH);
    char* puerto_interrupt = string_itoa(config_kernel->PUERTO_CPU_INTERRUPT);
    char* ip_cpu = config_kernel->IP_CPU;

    md_interrupt_cpu = crear_conexion("DISPATCHER", ip_cpu, puerto_dispatch);
    md_dispatch_cpu = crear_conexion("INTERRUPT", ip_cpu, puerto_interrupt);

    if(md_dispatch_cpu == -1 || md_interrupt_cpu == -1) {
        log_error(logger, "No se pudo conectar con la CPU");
        exit(-1);
    }

    config_kernel -> SOCKET_DISPATCH = md_dispatch_cpu;
    config_kernel -> SOCKET_INTERRUPT = md_interrupt_cpu; 
}

void generar_conexion_con_memoria(void) {
    int md_memoria = 0;

    char* puerto_memoria = string_itoa(config_kernel->PUERTO_MEMORIA);
    char* ip_memoria = config_kernel->IP_MEMORIA;

    md_memoria = crear_conexion("MEMORIA", ip_memoria, puerto_memoria);

    if(md_memoria == -1) {
        log_error(logger, "No se pudo conectar con la Memoria");
        exit(-1);
    }

    config_kernel -> SOCKET_MEMORIA = md_memoria;   
}