#include "estructuras.h"


void manejadorDeHilos(){
	int socketCliente;
	pthread_t thread_id;

	// Funcion principal
	while((socketCliente = aceptarConexionDeCliente(socketListener))) { 	// hago el accept

		//Creo hilo atendedor
		if( pthread_create( &thread_id , NULL, atenderNotificacion , (void*) &socketCliente) < 0){
			//log_error(..);
			perror("could not create thread");
			exit(-1);
		}

		//pthread_join();
	}

	//Chequeo que no falle el accept
	chequeoSocket(socketCliente);
}

void *atenderNotificacion(void * paqueteSocket){

	int socket = *(int*)paqueteSocket;
	int nroNotificacion = recvDeNotificacion(socket);

	switch(nroNotificacion){

		case ES_COORDINADOR:
			log_warning(logger, "La conexion recibida es erronea");
			close(socket);
			break;

		case ES_PLANIFICADOR:
			printf("Se ha conectado el planificador\n");
			log_info(logger,"Se ha conectado un planificador");
			socketPlanificador = socket;
			sendDeNotificacion(socket, ES_COORDINADOR);
			break;

		case ES_ESI:
			//list_add(ESIsconectados, socket);
			manejarESI(socket);
			break;

		case ES_INSTANCIA:
			list_add(instanciasConectadas,(void*) socket);		// Agrego a la lista la instancia conectada
			manejarInstancia(socket);		//Mando sendDeNotificacion, tamaño de estructuras.
			break;

		case 0:
			printf("El socket %d corto la conexion", socket);
			log_warning(logger, "El socket %d corto la conexion", socket);
			close(socket);
			break;

		default:
			printf("La conexion recibida es erronea");
			log_warning(logger, "La conexion recibida es erronea");
			close(socket);
			break;

	}

	return 0;

}

void manejarInstancia(int socket){
	printf("Se ha conectado una instancia\n");
	log_info(logger,"Se ha conectado una instancia");
	// HANDSHAKE
	sendDeNotificacion(socket, ES_COORDINADOR);
	// ENVIO TAMANIO DE ENTRADAS Y LA CANTIDAD
	log_info(logger,"Envio TAMAÑO Y CANTIDAD DE ENTRADAS");
	int tamanioInfo = 2*sizeof(int);
	void* infoAdministrativa = malloc(tamanioInfo);
	memcpy(infoAdministrativa, &CANTIDAD_ENTRADAS,sizeof(int));
	memcpy(infoAdministrativa+sizeof(int), &TAMANIO_ENTRADA,sizeof(int));
	sendRemasterizado(socket,DATOS_ADMINISTRATIVOS,tamanioInfo,infoAdministrativa);
	free(infoAdministrativa);
}

void manejarESI(int socket){
	printf("Se ha conectado un ESI\n");
	log_info(logger,"Se ha conectado un ESI");
	sendDeNotificacion(socket, ES_COORDINADOR);

	int corte = 1;

	while(corte){
		int nroOperacion = recvDeNotificacion(socket);

	 	switch(nroOperacion){

	 		case OPERACION_GET:{
	 			char* claveNueva = string_new();
	 			claveNueva = recibirString(socket);

	 			if(string_length(claveNueva) > TAMANIO_MAX_CLAVE){
	 				sendDeNotificacion(socketPlanificador,ERROR_TAMANIO_CLAVE);
	 			}

	 			//mutex_wait();
	 			list_add(clavesBloqueadas,(void*) claveNueva);
	 			//mutex_signal();


	 			break;
	 		}

	 		case OPERACION_SET:
	 			//int socketInstancia = retornarInstancia();

	 			break;

	 		case OPERACION_STORE:
	 			//retornarInstancia();
	 			break;

	 		default:
	 			break;


	 	}

	}

}


