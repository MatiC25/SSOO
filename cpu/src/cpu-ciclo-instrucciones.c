
int seguir_ejecutando;

void iniciar_ciclo_de_ejecucion(int socket_server) {
   int socket_cliente = esperar_cliente("INTERRUPT", socket_server);

    while(1) {
        op_code codigo_operacion = recibir_operacion(socket_cliente);

        switch(codigo_operacion) {
            // case HANDSHAKE:
            case EJECUTAR_INSTRUCCIONES:
                ejecutar_ciclo_instrucciones();
                break;
       }
    }
}

void ejecutar_ciclo_instrucciones(int socket_cliente) {
    t_instruccion *instruccion;
    seguir_ejecutando = 0;
    t_pcb_cpu *pcb = recibir_pcb(socket_cliente);

    while(!seguir_ejecutando) {
        fecth(pcb);
        ejecutar_instruccion(pcb);
    }
}

void fecth(pcb) {
    int PID = pcb->PID;
    int program_counter = pcb->program_counter;

    solicitar_instruccion(PID, program_counter);
}

void ejecutar_instruccion(t_pcb_cpu *pcb) {
    t_instruccion *instruccion = recv_instruccion();
    t_tipo_instruccion tipo_instruccion = decode(instruccion);

    switch(tipo_instruccion) {
        case SET:
            ejecutar_set(instruccion);
            break;
        case GET:
            ejecutar_get(instruccion);
            break;
        case SUM:
            ejecutar_sum(instruccion);
            break;
        case SUB:
            ejecutar_sub(instruccion);
            break;
        case JNZ:
            ejecutar_jnz(instruccion);
            break;
        case IO_GEN_SLEEP:
            ejecutar_io_gen_sleep(instruccion);
            break;
    }
}


void ejecutar_sum(t_instruccion *instruccion, t_pcb_cpu *pcb) {
    tipo_registro* registro_origen = obtener_registro(pcb, instruccion->parametros[0]);
    tipo_registro* registro_destino = obtener_registro(pcb, instruccion->parametros[1]);

    int valor = registro_origen->valor + registro_destino->valor;
    ejecutarset(instruccion->parametros[1], valor); 
}

t_registro* obtener_registro(t_pcb_cpu *pcb, char *registro) {
    if(strcmp(registro, "A") == 0) {
        return pcb->registro_A;
    } else if(strcmp(registro, "B") == 0) {
        return pcb->registro_B;
    } else if(strcmp(registro, "C") == 0) {
        return pcb->registro_C;
    } else if(strcmp(registro, "D") == 0) {
        return pcb->registro_D;
    } else if(strcmp(registro, "E") == 0) {
        return pcb->registro_E;
    } else if(strcmp(registro, "M") == 0) {
        return pcb->registro_M;
    }
}