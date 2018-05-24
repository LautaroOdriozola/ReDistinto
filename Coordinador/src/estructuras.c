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
			log_info(logger,"Se ha conectado un planificador");
			socketPlanificador = socket;
			sendDeNotificacion(socket, ES_COORDINADOR);
			break;

		case ES_ESI:
			//list_add(ESIsconectados, socket);
			manejarESI(socket);
			break;

		case ES_INSTANCIA:
			agregarInstancia(socket);	// Agrego a la lista la instancia conectada
			manejarInstancia(socket);		//Mando sendDeNotificacion, tamaño de estructuras.
			break;

		case 0:
			log_warning(logger, "El socket %d corto la conexion", socket);
			close(socket);
			break;

		default:
			log_warning(logger, "La conexion recibida es erronea");
			close(socket);
			break;

	}

	return 0;

}

void manejarInstancia(int socket){
	log_info(logger,"Se ha conectado una instancia");
	// HANDSHAKE
	sendDeNotificacion(socket, ES_COORDINADOR);
	// ENVIO TAMANIO DE ENTRADAS Y LA CANTIDAD
	log_info(logger,"Envio TAMAÑO Y CANTIDAD DE ENTRADAS");
	int tamanioInfo = 2*sizeof(uint32_t);
	void* infoAdministrativa = malloc(tamanioInfo);
	memcpy(infoAdministrativa, &CANTIDAD_ENTRADAS,sizeof(uint32_t));
	memcpy(infoAdministrativa+sizeof(uint32_t), &TAMANIO_ENTRADA,sizeof(uint32_t));
	sendRemasterizado(socket,DATOS_ADMINISTRATIVOS,tamanioInfo,infoAdministrativa);
	free(infoAdministrativa);
}

void manejarESI(int socket){
	log_info(logger,"Se ha conectado un ESI");
	sendDeNotificacion(socket, ES_COORDINADOR);

	int corte = 1;

	while(corte){
		int nroOperacion = recvDeNotificacion(socket);

	 	switch(nroOperacion){

	 		case OPERACION_GET:{
	 			manejarOperacionGet(socket);
	 			break;
	 		}

	 		case OPERACION_SET:{
	 			manejarOperacionSet(socket);
	 			break;
	 		}

	 		case OPERACION_STORE:{
	 			//int socketInstancia = retornarInstancia();
	 			break;
	 		}

	 		/*
	 		case 0:{
				log_warning(logger, "Se murio un ESI...", socket);
				close(socket);
				break;
	 		}*/

	 		default:
				//log_warning(logger, "OPERACION no reconocida", socket);
				//close(socket);
	 			break;


	 	}

	}

}

void manejarOperacionGet(int socket){

	char* claveNueva = string_new();
	claveNueva = recibirString(socket);

	// Cacheo Error de tamaño de clave
	if(string_length(claveNueva) > TAMANIO_MAX_CLAVE){
		log_warning(logger, "Tamaño de la clave es mayor a 40 caracteres. Mandando a matar al ESI");
		//sendDeNotificacion(socket,ERROR_TAMANIO_CLAVE); SOCKET DEL ESI O SOCKET DE PLANIFICADOR?!?!?
	}

	//mutex_wait();
	list_add(clavesBloqueadas,(void*) claveNueva);
	//mutex_signal();

	int tamanio = 0;
	void * claveBloqueada = malloc(string_length(claveNueva) + sizeof(uint32_t));
	int tamanioClave = string_length(claveNueva);
	memcpy(claveBloqueada + tamanio, &tamanioClave , sizeof(uint32_t));
	tamanio += sizeof(uint32_t);
	memcpy(claveBloqueada + tamanio, claveNueva, string_length(claveNueva));
	tamanio += string_length(claveNueva);

	sendRemasterizado(socketPlanificador,OPERACION_GET,tamanio,claveBloqueada);
	free(claveBloqueada);
}

void manejarOperacionSet(int socket){
	int socketInstancia = retornarInstancia();

	char * claveNueva = string_new();
	char * valorParaAlmacenar = string_new();

	claveNueva = recibirString(socket);
	valorParaAlmacenar = recibirString(socket);

	// @ PARA TESTEAR
	//string_append(&claveNueva,"futbol:jugador:messi");
	//string_append(&valorParaAlmacenar, "LionelMessi");

	int largoClave =  string_length(claveNueva);
	int largoValor = string_length(valorParaAlmacenar);

	int tamanio = 0;
	//En el buffer mando clave y luego valor
	void* buffer = malloc(string_length(claveNueva) + string_length(valorParaAlmacenar) + 2*sizeof(uint32_t));
	memcpy(buffer + tamanio, &largoClave, sizeof(uint32_t));
	tamanio += sizeof(uint32_t);
	memcpy(buffer + tamanio, claveNueva, string_length(claveNueva));
	tamanio += largoClave;
	memcpy(buffer + tamanio, &largoValor , sizeof(uint32_t));
	tamanio += sizeof(uint32_t);
	memcpy(buffer + tamanio, valorParaAlmacenar, string_length(valorParaAlmacenar));
	tamanio += largoValor;

	sendRemasterizado(socketInstancia,OPERACION_SET, tamanio, buffer);
	free(buffer);

	/*
	 *
	 * FALTA RESPUESTA DE INSTANCIA
	 *
	 */
}

int retornarInstancia(){

	int socketInstancia;

	if(strcmp(ALGORITMO, "LSU") == 0){
		//socketInstancia = getInstanciaLSU();
	} else if(strcmp(ALGORITMO, "KE") == 0){
		//socketInstancia = getInstanciaKE();
	} else if(strcmp(ALGORITMO, "EL") == 0){
		socketInstancia = getInstanciaEquitativeLoad();
	}

	return socketInstancia;
}

void agregarInstancia(int socket){
	infoAlgoritmoDistribucion * info = malloc(sizeof(infoAlgoritmoDistribucion));

	info->socketInstancia = socket;
	info->espacioUtilizado = 0;

	//mutex_wait()
	list_add(instanciasConectadas,info);
	//mutex_signal()

}

int getInstanciaEquitativeLoad(){

	int socketInstancia;

	// Cacheo si la lista esta vacia, devuelvo como socket un 0
	if(list_is_empty(instanciasConectadas)){
		log_error(logger, "No hay instancias para atender la solicitud");
		socketInstancia = 0;
	}

	//Saco el primer elemento de la lista y lo guardo.

	//mutex_wait
	infoAlgoritmoDistribucion * info = list_remove(instanciasConectadas,0);
	//mutex_signal

	socketInstancia = info->socketInstancia;

	//Guardo el elemento al final de la lista
	//mutex_wait
	list_add(instanciasConectadas,info);
	//mutex_signal

	return socketInstancia;

}



