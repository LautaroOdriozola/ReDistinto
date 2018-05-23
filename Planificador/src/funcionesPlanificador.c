#include "funcionesPlanificador.h"

//Consola PLANIFICADOR
void levantarConsolaPlanificador(){
	printf("Consola Iniciada. Ingrese una opcion \n");
	log_trace(logger, "Se inicializo la consola");
	 	char * linea;
		linea = readline(">");
		while(1) {
		linea = readline(">");
			if (!linea) {
				break;
			}else{
				if (strcmp(linea, "pausar") == 0){
					printf("Pausea la planificacion\n");
					free(linea);
				}else if (strcmp(linea, "continuar") == 0){
					printf("Continua la planificacion\n");
					free(linea);
				}else if (strcmp(linea, "bloquear [clave] [id]") == 0){
					printf("Se bloqueara el ESI hasta ser desbloqueado/n");
					free(linea);
				}else if (strcmp(linea, "desbloquear [clave]") == 0){
					printf("Se desbloqueara el primer proceso ESI bloqueado por clave\n");
					free(linea);
				}else if (strcmp(linea, "listar [recurso]") == 0){
					printf("Lista los procesos encolados esperando un recurso.\n");
					free(linea);
				}else if (strcmp(linea, "kill [ID]") == 0){
					printf("Mata un proceso\n");
					free(linea);
				}else if (strcmp(linea, "status [clave]") == 0){
					printf("informacion sobre la clave.\n");
					free(linea);
				}else if (strcmp(linea, "deadlock ") == 0){
					printf("Permite observar si hay un deadlock en el sistema\n");
					free(linea);
				}else {
					printf("Opcion no valida.\n");
					free(linea);
				}
			}
	 }
}


void cargarConfigPlanificador(t_config* configuracion){

	//char ** arrayClaves;

	if (!config_has_property(configuracion, "PUERTO_ESCUCHA")) {
		printf("No se encuentra el parametro PUERTO_ESCUCHA en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro PUERTO_ESCUCHA en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	PUERTO_ESCUCHA = config_get_int_value(configuracion,"PUERTO_ESCUCHA");
	printf("PUERTO_ESCUCHA=%d\n", PUERTO_ESCUCHA);


	if (!config_has_property(configuracion, "ALGORITMO_PLANIFICACION")) {
		printf("No se encuentra el parametro ALGORITMO_PLANIFICACION en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro ALGORITMO_PLANIFICACION en el archivo de configuracion");
		config_destroy(configuracion);
		//liberarMemoria();
		exit(-1);
	}

	ALGORITMO_PLANIFICACION = string_new();
	string_append(&ALGORITMO_PLANIFICACION, config_get_string_value(configuracion, "ALGORITMO_PLANIFICACION"));
	printf("ALGORITMO_PLANIFICACION=%s\n", ALGORITMO_PLANIFICACION);

	if (!config_has_property(configuracion, "ESTIMACION_INICIAL")) {
		printf("No se encuentra el parametro ESTIMACION_INICIAL en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro ESTIMACION_INICIAL en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	ESTIMACION_INICIAL = config_get_int_value(configuracion,"ESTIMACION_INICIAL");
	printf("ESTIMACION_INICIAL=%d\n", ESTIMACION_INICIAL);

	if (!config_has_property(configuracion, "COORDINADOR_IP")) {
		printf("No se encuentra el parametro COORDINADOR_IP en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro COORDINADOR_IP en el archivo de configuracion");
		config_destroy(configuracion);
		//liberarMemoria();
		exit(-1);
	}

	COORDINADOR_IP = string_new();
	string_append(&COORDINADOR_IP, config_get_string_value(configuracion, "COORDINADOR_IP"));
	printf("COORDINADOR_IP=%s\n", COORDINADOR_IP);

	if (!config_has_property(configuracion, "PUERTO_COORDINADOR")) {
		printf("No se encuentra el parametro PUERTO_COORDINADOR en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro PUERTO_COORDINADOR en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	PUERTO_COORDINADOR = config_get_int_value(configuracion,"PUERTO_COORDINADOR");
	printf("PUERTO_COORDINADOR=%d\n", PUERTO_COORDINADOR);

	if (!config_has_property(configuracion, "CLAVES_BLOQUEADAS")) {
		printf("No se encuentra el parametro CLAVES_BLOQUEADAS en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro CLAVES_BLOQUEADAS en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	CLAVES_BLOQUEADAS = config_get_array_value(configuracion,"CLAVES_BLOQUEADAS");
	int cantidadClavesBloqueadas = 0;

	while(CLAVES_BLOQUEADAS[cantidadClavesBloqueadas] != NULL){
		printf("CLAVE BLOQUEADA: %s \n", CLAVES_BLOQUEADAS[cantidadClavesBloqueadas]);
		cantidadClavesBloqueadas++;
	}

	config_destroy(configuracion);

}

void atenderConexion(int socketNuevo){
	int socketAceptado = aceptarConexionDeCliente(socketNuevo);
	FD_SET(socketAceptado, &socketClientes);
	socketMaximo = calcularSocketMaximo(socketAceptado,socketMaximo);
	log_info(logger,"Se ha recibido una nueva conexion de un ESI");
}


void atenderNotificacion(int socket){

	int nroNotificacion = recvDeNotificacion(socket);

	switch(nroNotificacion){

		case ES_COORDINADOR:
			log_warning(logger, "La conexion recibida es erronea");
			FD_CLR(socket, &socketClientes);
			close(socket);
			break;

		case ES_PLANIFICADOR:
			log_warning(logger, "La conexion recibida es erronea");
			FD_CLR(socket, &socketClientes);
			close(socket);
			break;

		case ES_ESI:
			printf("Se ha conectado un ESI\n");
			log_info(logger,"Se ha conectado un ESI");
			sendDeNotificacion(socket, ES_PLANIFICADOR);
			break;

		case ES_INSTANCIA:
			log_warning(logger, "La conexion recibida es erronea");
			FD_CLR(socket, &socketClientes);
			close(socket);
			break;

		case 0:
			log_warning(logger, "El socket %d corto la conexion", socket);
			FD_CLR(socket, &socketClientes);
			close(socket);
			break;

		default:
			log_warning(logger, "La conexion recibida es erronea");
			FD_CLR(socket, &socketClientes);
			close(socket);
			break;

	}

}

void manejarSelect(){
	//Creo servidor para los ESI
	socketListener = iniciarServidor(PUERTO_ESCUCHA);
	socketMaximo = socketListener;

	//Limpio estructuras
	FD_ZERO(&socketClientes);
	FD_ZERO(&socketClientesAuxiliares);
	FD_SET(socketListener, &socketClientes);

	int socketCliente;
	int corte = 1;

	while(corte){

		//pthread_mutex_lock(&mutex);
		socketClientesAuxiliares = socketClientes;
		//pthread_mutex_unlock(&mutex);

		if (select(socketMaximo + 1, &socketClientesAuxiliares, NULL, NULL,NULL) == -1) {
			log_error(logger, "No se pudo llevar a cabo el select");
			//liberarMemoria();
			exit(-1);
		}

		for (socketCliente = 0; socketCliente <= socketMaximo; socketCliente++) {

			//pthread_mutex_lock(&mutex);
			bool fd_isset = FD_ISSET(socketCliente, &socketClientesAuxiliares);

			if (fd_isset) {
				if (socketCliente == socketListener) {

					// Nueva conexion
					atenderConexion(socketCliente);

				} else {

					// Nueva notificacion
					atenderNotificacion(socketCliente);

				}

			}
			//pthread_mutex_unlock(&mutex);

		}

	}
}
