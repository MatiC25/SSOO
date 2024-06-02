void escuchar_peticiones_dispatch() {
    while(1) {
        // Recibimos la peticion desde el socket de dispatch:
        tipo_peticion peticion = recibir_peticion_dispatch();
        t_pcb *pcb = recibir_pcb_dispatch();

        // Ejecutamos la peticion recibida:
        diccionario_peticiones[peticion](pcb);
    }
}

void 