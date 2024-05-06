#include "protocolo.h"

t_paquete* crear_paquete(op_code operacion) {
	t_paquete* paquete = malloc(sizeof(t_paquete));
    paquete->codigo_operacion = operacion;
	crear_buffer(paquete);
	return paquete;
}

void crear_buffer(t_paquete* paquete) {
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio) {
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int));

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int));
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio);

	paquete->buffer->size += tamanio + sizeof(int);
	
	// paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);
	// memcpy(paquete->buffer->stream + paquete->buffer->size, valor, tamanio);
	// paquete->buffer->size += tamanio;
}

void enviar_paquete(t_paquete* paquete, int socket_cliente) {
	int bytes = paquete->buffer->size + sizeof(op_code) + sizeof(uint32_t);
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void* serializar_paquete(t_paquete* paquete, int* bytes) {
	void * magic = malloc(*bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(op_code)); //Creo qeu hay que sacarlo
	desplazamiento += sizeof(op_code);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(uint32_t));
	desplazamiento += sizeof(uint32_t);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);

	return magic;
}

void eliminar_paquete(t_paquete* paquete) {
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void* recibir_buffer(int* size, int socket_cliente) {
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

int recibir_operacion(int socket_cliente)
{
	int cod_op;
	if (recv(socket_cliente, &cod_op, sizeof(int), MSG_WAITALL) > 0)
		return cod_op;
	else
	{
		close(socket_cliente);
		return -1;
	}
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1;
	paquete->buffer->stream = malloc(paquete->buffer->size);
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size);

	int bytes = paquete->buffer->size + 2 * sizeof(int);
	void* a_enviar = serializar_paquete(paquete, bytes);
	int bytes_enviados = send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}

void recibir_mensaje(int socket_cliente)
{
	int size;
	char *buffer = recibir_buffer(&size, socket_cliente);
	log_info(logger, "Me llego el mensaje %s", buffer);
	free(buffer);
}


void send_contexto_ejecucion(op_code operacion, int socket_cliente, t_pcb* proceso) {
    t_paquete* paquete = crear_paquete(operacion);
    agregar_a_paquete_PCB(paquete, proceso);
    agregar_a_paquete_registros(paquete, proceso->registros);
    agregar_a_paquete_lista_string(paquete, proceso->archivos_abiertos);

    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void agregar_a_paquete_PCB(t_paquete* paquete, t_pcb* proceso) {
    agregar_a_paquete(paquete, &proceso->pid, sizeof(int)); 
    agregar_a_paquete(paquete, &proceso->program_counter, sizeof(int)); // PARA MI SE COMENTA
}

void agregar_a_paquete_registros(t_paquete* paquete, t_registros_cpu* registros) {
	agregar_a_paquete(paquete, &registros->PC, sizeof(uint32_t)); 
	agregar_a_paquete(paquete, &registros->AX, sizeof(uint32_t));
    agregar_a_paquete(paquete, &registros->BX, sizeof(uint8_t));
    agregar_a_paquete(paquete, &registros->CX, sizeof(uint8_t));
    agregar_a_paquete(paquete, &registros->DX, sizeof(uint8_t));
	agregar_a_paquete(paquete, &registros->EAX, sizeof(uint32_t));
	agregar_a_paquete(paquete, &registros->EBX, sizeof(uint32_t));
	agregar_a_paquete(paquete, &registros->ECX, sizeof(uint32_t));
	agregar_a_paquete(paquete, &registros->EDX, sizeof(uint32_t));
	// agregar_a_paquete(paquete, &registros->SI, sizeof(uint32_t)); HACE FALTA LOS PUNTEROS A STRING 
	// agregar_a_paquete(paquete, &registros->DI, sizeof(uint32_t));


}

void agregar_a_paquete_lista_string(t_paquete* paquete, t_list* archivos_abiertos) {
    int cantidad_archivos = list_size(archivos_abiertos);

    for(int i = 0; i < cantidad_archivos; i++) {
        char* archivo = list_get(archivos_abiertos, i);
        agregar_a_paquete_string(paquete, archivo, strlen(archivo) + 1);
    }
}

void agregar_a_paquete_string(t_paquete* paquete, char* cadena, int tamanio) {
    int cadena_length = string_length(cadena);
    size_t size = sizeof(int);  // Tamaño en bytes de un entero
    
    // Expandir el tamaño del buffer del paquete para acomodar la longitud de la cadena
    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + size);
    memcpy(paquete->buffer->stream + paquete->buffer->size, &cadena_length, size);
    paquete->buffer->size += size;

    // Expandir el tamaño del buffer para acomodar la cadena
    paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio);
    memcpy(paquete->buffer->stream + paquete->buffer->size, cadena, tamanio);
    paquete->buffer->size += tamanio;
}


t_pcb* rcv_contexto_ejecucion(int socket_cliente) {
    t_pcb* proceso = malloc(sizeof(t_pcb));
    proceso->registros = malloc(sizeof(t_registros_cpu));
    proceso->archivos_abiertos = list_create();

    // PCB -> PID, PC, Registros, Archivos abiertos
    int size;
    int desplazamiento = 0;
    void* buffer = recibir_buffer(&size, socket_cliente);

    memcpy(&proceso->pid, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    memcpy(&proceso->program_counter, buffer + desplazamiento, sizeof(int));
    desplazamiento += sizeof(int);

    // memcpy(&proceso->registros->PC, buffer + desplazamiento, sizeof(uint32_t));
    // desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->AX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&proceso->registros->BX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&proceso->registros->CX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

    memcpy(&proceso->registros->DX, buffer + desplazamiento, sizeof(uint8_t));
    desplazamiento += sizeof(uint8_t);

	memcpy(&proceso->registros->EAX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

	memcpy(&proceso->registros->EBX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

	memcpy(&proceso->registros->ECX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

	memcpy(&proceso->registros->EDX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);
	
	memcpy(&direccion_SI, buffer + desplazamiento, sizeof(uint32_t *));
	desplazamiento += sizeof(uint32_t *);

	memcpy(&direccion_DI, buffer + desplazamiento, sizeof(uint32_t *));
	desplazamiento += sizeof(uint32_t *);

    free(buffer);
    return proceso;
}

t_list *recv_list(int socket_cliente) {
	int size;
	int desplazamiento = 0;
	t_list *lista = list_create();
	void* buffer = recibir_buffer(&size, socket_cliente);

	while(desplazamiento < size) {
		int tamanio;
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);

		char* archivo = malloc(tamanio);
		memcpy(archivo, buffer + desplazamiento, tamanio);
		desplazamiento += tamanio;

		list_add(lista, archivo);
	}

	free(buffer);

	return lista;
}