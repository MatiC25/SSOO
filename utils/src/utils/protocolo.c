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

void* serializar_paquete(t_paquete* paquete, int bytes) {
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(op_code));
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
    agregar_a_paquete_archivos_abiertos(paquete, proceso->archivos_abiertos);

    enviar_paquete(paquete, socket_cliente);
    eliminar_paquete(paquete);
}

void agregar_a_paquete_PCB(t_paquete* paquete, t_pcb* proceso) {
    agregar_a_paquete(paquete, &proceso->pid, sizeof(int));
    agregar_a_paquete(paquete, &proceso->program_counter, sizeof(int));
}

void agregar_a_paquete_registros(t_paquete* paquete, t_registros_cpu* registros) {
    agregar_a_paquete(paquete, &registros->AX, sizeof(uint32_t));
    agregar_a_paquete(paquete, &registros->BX, sizeof(uint32_t));
    agregar_a_paquete(paquete, &registros->CX, sizeof(uint32_t));
    agregar_a_paquete(paquete, &registros->DX, sizeof(uint32_t));
}

void agregar_a_paquete_archivos_abiertos(t_paquete* paquete, t_list* archivos_abiertos) {
    int cantidad_archivos = list_size(archivos_abiertos);

    for(int i = 0; i < cantidad_archivos; i++) {
        char* archivo = list_get(archivos_abiertos, i);
        printf("Archivo abierto: %s\n", archivo);
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

    memcpy(&proceso->registros->AX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->BX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->CX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    memcpy(&proceso->registros->DX, buffer + desplazamiento, sizeof(uint32_t));
    desplazamiento += sizeof(uint32_t);

    int tamanio;

    while(desplazamiento < size) {

        memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
        desplazamiento += sizeof(int);
        char* archivo = malloc(tamanio);
        memcpy(archivo, buffer + desplazamiento, tamanio);
        desplazamiento += tamanio + 1;

        list_add(proceso->archivos_abiertos, archivo);
    }


    free(buffer);
    return proceso;
}

t_list* recibir_paquete(int socket_cliente)
{
	int tamanio;
	int size;
	int desplazamiento = 0;
	void * buffer;
	
	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);

		t_list* valores = list_create();
		char* valor = malloc(tamanio);
		memcpy(valor, buffer + desplazamiento, tamanio);
		desplazamiento += tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

void paquete(int conexion)
{
	// Ahora toca lo divertido!
	char* leido = NULL;
	op_code code = PAQUETE;
	t_paquete* paquete = crear_paquete(code);
	printf("Escribi lo que quieras que sea mandado el servidor para comunicarte con el..\n");
	log_info(logger,"-->  Comunicaciones con el servidor  <--");
	leido = readline("--> ");
	log_info(logger,"--> %s",leido); //opcional 
	// Leemos y esta vez agregamos las lineas al paquete
	do{

		agregar_a_paquete(paquete, leido, strlen(leido) + 1 );
		
		free(leido);
		leido = readline("--> ");
		log_info(logger,"--> %s",leido); //opcional 
	}while (strcmp(leido,"")!= 0);

	free(leido);
	enviar_paquete(paquete,conexion);

	// ¡No te olvides de liberar las líneas y el paquete antes de regresar!
	eliminar_paquete(paquete);
}

//Creamos una funcion que envie el archivo pseudo y el pid del proceso desde kernel a memoria para que pueda ser utilizado en la lectura de pseudocodigo

void send_archi_pid(int socket_cliente, char *path, int pid) {
    int tamanio = strlen(path) + 1; //Calculamos el largo de archivo_path
    int size = sizeof(int) + tamanio + sizeof(int);  //Arbitrariamente elegimos el largo del paquete a enviar ya que sabemos lo que mandamos
    void *buffer = malloc(size); //Reservamos memoria para el buffer
    int desplazamiento = 0; //Init del desplazamiento que será utilizado para leer dato a dato

    memcpy(buffer + desplazamiento, &tamanio, sizeof(int)); //Copiamos el dato, ente caso del tamaño de un int
    desplazamiento += sizeof(int); //Desplzamos el puntero para que avance al siguiente bloque de datos, sin pisar el dato anterior

    memcpy(buffer + desplazamiento, path, tamanio); 
    desplazamiento += tamanio; 

    memcpy(buffer + desplazamiento, &pid, sizeof(int));

    enviar_buffer(buffer, size, socket_cliente); //Enviamos el buffer con el tamaño del paquete a enviar que será recibido luego
    free(buffer); // Liberamos el buffer xq no somos petes

//Creamos una funcion que reciba el archivo pseudo y el pid del proceso que será utilizado para la lectura de pseudocodigo de instrucciones en memoria 

void recv_archi_pid(int socket_cliente, char **path, int* pid){ // Se usan punteros xq necesitamos modificar las variables que se pasan por parametro
	int size;
	int desplazamiento = 0;
	void *buffer;
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente); //Recibimos el buffer que antes enviamos
	while (desplazamiento < size) // Leemos el buffer hasta que lleguemos al final
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int)); //Averiguamos que tan largo es el string (path)
		desplazamiento += sizeof(int);
		path = malloc(tamanio); // Una vez que sabemos el largo, podemos reservar el espacio de memoraia del mismo 
								
		memcpy(*path, buffer + desplazamiento, tamanio); // Copiamos todo el path de una en la variable "*path"
		desplazamiento += tamanio;

		memcpy(pid, buffer + desplazamiento, sizeof(int));
		desplazamiento += sizeof(int);

	}
	free(buffer);
}

//Funcion para recibir el PC y el PID de un proceso
recibir_program_counter(int socket_cpu,int *pid,int *program_counter){

    int tamaño;
    int desplazamiento = 0;
    void *buffer;

    buffer = recibir_buffer(&tamaño, socket_cpu); //Si llega a fallar funcion recibir_paquete
    while(desplazamiento < tamaño){

        memcpy(program_counter, buffer + desplazamiento, sizeof(int));//memcpy copia los datos del buffer al program counter
        desplazamiento += sizeof(int);//el desplazamiento se incrementa en el tamaño de un entero para moverse al siguiente conjunto de datos en el buffer.
                                      															
        memcpy(pid, buffer+desplazamiento, sizeof(int));//copiar los datos del bufer las pid
        desplazamiento += sizeof(int);
    }
    free(buffer);
}

//Retardo pedido

void retardo_pedido(int tiempo_de_espera){
    usleep(tiempo_de_espera*1000) //*1000 es para pasarlo a milisegundos ya que usleep usa microsegundos
    //cambiar por el cofig
}