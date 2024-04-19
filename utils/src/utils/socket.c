#include "socket.h"

int iniciar_servidor(t_log* logger, const char* name, char* ip, char* puerto) 
{
    int socket_servidor;
    struct addrinfo hints, *servinfo;

    // Inicializando hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    // Recibe los addrinfo
    getaddrinfo(ip, puerto, &hints, &servinfo);

    int conecto = 0;

    // Itera por cada addrinfo devuelto
    for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next) {
        socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_servidor == -1) // fallo de crear socket
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            // Si entra aca fallo el bind
            close(socket_servidor);
            continue;
        }
        // Ni bien conecta uno nos vamos del for
        conecto = 1;
        break;
    }

    if(!conecto) {
        free(servinfo);
        return 0;
    }

    listen(socket_servidor, SOMAXCONN); // Escuchando (hasta SOMAXCONN conexiones simultaneas)

    // Aviso al logger
    log_info(logger, "Escuchando en %s:%s (%s)\n", ip, puerto, name);

    freeaddrinfo(servinfo);

    return socket_servidor;
}

int esperar_cliente(t_log *logger, const char *name, int socket_servidor) //name -> quién se conecta
{
	struct sockaddr_in dir_cliente;
	socklen_t tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void *)&dir_cliente, &tam_direccion);

	log_info(logger, "¡Cliente %s conectado!\n", name);

	return socket_cliente;
}

int crear_conexion(t_log *logger, const char *server_name, char *ip, char *puerto)
{
	struct addrinfo hints, *servinfo;

	// Init de hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	// Recibe addrinfo
	getaddrinfo(ip, puerto, &hints, &servinfo);

	// Crea un socket con la informacion recibida (del primero, suficiente)
	int socket_cliente = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);

	printf("Socket cliente: %d\n", socket_cliente);

	// Fallo en crear el socket
	if (socket_cliente == -1)
	{
		log_error(logger, "Error creando el socket para %s:%s", ip, puerto);
		return 0;
	}

	// Error conectando
	if (connect(socket_cliente, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		log_error(logger, "Error al conectar (a %s)\n", server_name);
		freeaddrinfo(servinfo);
		return 0;
	}
	else
		log_info(logger, "Cliente conectado en %s:%s (a %s)\n", ip, puerto, server_name);

	freeaddrinfo(servinfo);

	return socket_cliente;
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}

void atender_conexion(t_log* logger, char* server_name, int cliente_socket) 
{
	t_list* lista;

	while (1) 
	{
		op_code cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) 
		{
			case MENSAJE:
				recibir_mensaje(logger, cliente_socket);
				break;
			//case PAQUETE:
				//lista = recibir_paquete(cliente_socket);
				//log_info(logger, "Me llegaron los siguientes valores:\n");
				//list_iterate(lista, (void*) iterator);
				//ietrar lista y mostart por logger.
				//break;
			//case -1:
			// 	log_error(logger, "el cliente se desconecto. Terminando servidor");
			// 	return EXIT_FAILURE;
			// default:
				// 	log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			// 	break;
		//}
		}
	
    
	}
	return;
}


// void iterator(char* value) {
// 	log_info(logger,"%s", value);



void server_escuchar_sin_hilos(void* args) 
{

    t_procesar_server* args_hilo = (t_procesar_server*) args;
    t_log* logger_server = args_hilo->logger;
    char* server_name = args_hilo->server_name;
    int socket_server = args_hilo->socket_servidor;

    while (1)
    {
        int socket_cliente = esperar_cliente(logger_server, server_name, socket_server);
        
        if(socket_cliente != -1){
            atender_conexion(logger_server, server_name, socket_cliente);
    	}
	}

}

void server_escuchar_con_hilos(t_log* logger, char* server_name, int socket_server) 
{
    while (1)
    {
        int socket_cliente = esperar_cliente(logger, server_name, socket_server);

        if(socket_cliente != -1) 
        {
          
			pthread_t hilo;
			t_procesar_conexion *args_hilo = crear_procesar_conexion(logger, server_name, socket_cliente);
      
            pthread_create(&hilo, NULL, (void*) atender_conexiones_memoria, (void *) args_hilo);
            pthread_detach(hilo);            
			
        }
    }

}

void atender_conexiones_memoria(void *args)
{
	t_procesar_conexion *args_hilo = (t_procesar_conexion *)args;
	t_log *logger = args_hilo->logger;
	char *server_name = args_hilo->server_name;
	int cliente_socket = args_hilo->socket_cliente;
	
	while (1) 
	{
		op_code cod_op = recibir_operacion(cliente_socket);

		switch (cod_op) 
		{
			case MENSAJE:
				recibir_mensaje(logger, cliente_socket);
				break;
		}
	}
	
	log_warning(logger, "El cliente se desconecto de %s server", server_name);

	return;
}

