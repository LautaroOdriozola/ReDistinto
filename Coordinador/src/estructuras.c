#include "estructuras.h"


void manejadorDeHilos(){
	int socketCliente;
	pthread_t thread_id;

	// Funcion principal
	while((socketCliente = aceptarConexionDeCliente(socketListener))) { 	// hago el accept

		//Creo hilo atendedor
		if( pthread_create( &thread_id , NULL, atenderNotificacion , (void*) &socketCliente) < 0){
			log_error(logger,"No puedo crear mas hilos!");
			exit(-1);
		}

		pthread_join(thread_id,NULL);
	}

	//Chequeo que no falle el accept
	chequeoSocket(socketCliente);
}

void *atenderNotificacion(void * paqueteSocket){

	int socket = *(int*)paqueteSocket;
	uint32_t nroNotificacion = recvDeNotificacion(socket);

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
	log_trace(logger,"Se ha conectado una instancia");

	agregarInstancia(socket);	// Agrego a la lista la instancia conectada

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
	log_trace(logger,"Se ha conectado un ESI");
	sendDeNotificacion(socket, ES_COORDINADOR);

	uint32_t ID_ESI = recibirUint(socket);		// Recibo el ID del ESI conectado.
	log_info(logger, "Se conecto el ESI nro: %d", ID_ESI);

	agregarESI(socket, ID_ESI);

	int corte = 1;

	while(corte){
		uint32_t nroOperacion = recvDeNotificacion(socket);

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

	log_debug(logger, "REALIZANDO OPERACION GET!");
	char* claveNueva = recibirString(socket);


	/*
	// Cacheo Error de tamaño de clave
	if(string_length(claveNueva) > TAMANIO_MAX_CLAVE){
		log_warning(logger, "Tamaño de la clave es mayor a 40 caracteres. Mandando a matar al ESI");
		sendDeNotificacion(socket,ERROR_TAMANIO_CLAVE); // SOCKET DEL ESI
	}*/

	int tamanio = 0;
	void * claveBloqueada = malloc(string_length(claveNueva) + sizeof(uint32_t));
	int tamanioClave = string_length(claveNueva);
	memcpy(claveBloqueada + tamanio, &tamanioClave , sizeof(uint32_t));
	tamanio += sizeof(uint32_t);
	memcpy(claveBloqueada + tamanio, claveNueva, string_length(claveNueva));
	tamanio += string_length(claveNueva);

	sendRemasterizado(socketPlanificador,OPERACION_GET,tamanio,claveBloqueada);
	free(claveBloqueada);

	// int respuesta = recvDeNotificacion(socketPlanificador);		// rta del planificador por si pudo guardar la clave.
	// sendDeNotificacion(socket, respuesta);		// Le digo si pude realizar todo bien.


}

void manejarOperacionSet(int socket){

	log_debug(logger, "REALIZANDO OPERACION SET!");
	int id = buscarIdEsi(socket);

	char * claveNueva = recibirString(socket);
	log_info(logger, "Recibo clave %s por parte del ESI NRO = %d", claveNueva, id);
	char * valorParaAlmacenar = recibirString(socket);
	log_info(logger, "Recibo valor %s por parte del ESI NRO = %d", valorParaAlmacenar, id);


	if(existeClave(claveNueva)){
		infoAlgoritmoDistribucion * instancia = elegirInstancia(claveNueva);
		int socketInstancia = instancia->socketInstancia;

		log_debug(logger,"!!!!!!!!!!PROCESANDO INSTRUCCION!!!!!!!!!!!!!");
		sleep(RETARDO/1000);

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

		int resultadoOperacionSet = recvDeNotificacion(socketInstancia);
		log_info(logger,"Recibo RESPUESTA de la INSTANCIA elegida.");

		if(resultadoOperacionSet == OPERACION_EXITO){
			sendDeNotificacion(socket, OPERACION_EXITO);
			log_trace(logger, "OPERACION SET realizada con EXITO!");
			log_info(logger, "Envio RESPUESTA de operacion a ESI NRO = %d", id);

			//LOGUEAR RESPUESTA EN CASO DE SER EXITOSA
			//loguearRespuestaSet(id,claveNueva,valorParaAlmacenar);
			log_info(logger, "Logueando operacion en LOG DE OPERACIONES para el ESI NRO = %d", id);

			/***********************************************/
			// agregarClave(socketInstancia, claveNueva);
			// aumentarCantidadDeEntradasEscritas(socketInstancia);

			/************************************************/

		} else{
			log_error(logger,"Informando al ESI NRO = %d que no se pudo realizar la OPERACION SET", id);
			sendDeNotificacion(socket, ERROR_DE_INSTANCIA);
		}

	} else{
		log_error(logger, "ERROR DE CLAVE NO IDENTIFICADA! ABORTANDO ESI NRO = %d", id);
		sendDeNotificacion(socket, ERROR_CLAVE_NO_IDENTIFICADA);
		close(socket);
	}

}

infoAlgoritmoDistribucion*	elegirInstancia(char* claveNueva){

	infoAlgoritmoDistribucion * instancia;

	if(strcmp(ALGORITMO, "LSU") == 0){
		//socketInstancia = getInstanciaLSU();
	} else if(strcmp(ALGORITMO, "KE") == 0){
		//socketInstancia = getInstanciaKE();
	} else if(strcmp(ALGORITMO, "EL") == 0){
		instancia = getInstanciaEquitativeLoad(claveNueva);
	}

	return instancia;
}

void agregarInstancia(int socket){
	infoAlgoritmoDistribucion * info = malloc(sizeof(infoAlgoritmoDistribucion));

	info->socketInstancia = socket;
	info->clavesTomadas = list_create();
	info->cantidadEntradasEscritas = 0;

	//mutex_wait()
	list_add(instanciasConectadas,info);
	//mutex_signal()
	log_info(logger,"Enlisto nueva INSTANCIA conectada");
}

void agregarESI(int socket, int ID_ESI){
	infoESI * info = malloc(sizeof(infoESI));

	info->socket = socket;
	info->ID_ESI = ID_ESI;

	//mutex_wait();
	list_add(ESIsConectados,info);
	//mutex_signal();
	log_info(logger, "Enlisto nuevo ESI conectado");
}


// PROBAR!!
bool existeClave(char* claveNueva){

	bool _existeClaveEnLista(infoAlgoritmoDistribucion * info){
		return existeClaveEnLista(info->clavesTomadas,claveNueva);
	}

	//Me fijo si existe la clave tomada en alguna instancia
	return list_any_satisfy(instanciasConectadas, (void*) _existeClaveEnLista);

}

// PROBAR!
bool existeClaveEnLista(t_list * lista, char* clave){

	//Funcion aux para comparar claves ya tomadas
	bool buscarClave(char* claveNueva){
		return string_contains(claveNueva, clave);
	}

	return list_any_satisfy(lista, (void*) buscarClave);
}


//PROBAR!!
int buscarIdEsi(int socket){

	bool buscarSocket(infoESI* info){
		return (info->socket == socket);
	}

	infoESI* data = list_find(ESIsConectados,(void*) buscarSocket);

	return data->ID_ESI;

}


infoAlgoritmoDistribucion* getInstanciaEquitativeLoad(char* clave){

	// Cacheo si la lista esta vacia, devuelvo como socket un 0
	if(list_is_empty(instanciasConectadas)){
		log_error(logger, "No hay instancias para atender la solicitud");
	}

	//Saco el primer elemento de la lista y lo guardo.

	//mutex_wait
	infoAlgoritmoDistribucion * info = list_remove(instanciasConectadas,0);
	//mutex_signal

	//Seteo la clave tomada por esa instancia
	//info->claveTomada = clave;

	//Guardo el elemento al final de la lista
	//mutex_wait
	list_add(instanciasConectadas,info);
	//mutex_signal

	return info;

}

void agregarClave(int socket, char* clave){

	bool buscarSocket(infoAlgoritmoDistribucion* info){
		return (info->socketInstancia == socket);
	}

	infoAlgoritmoDistribucion * data = list_find(instanciasConectadas, (void*) buscarSocket);

	//mutex_wait();
	list_add(data->clavesTomadas, clave);
	//mutex_signal();

}





