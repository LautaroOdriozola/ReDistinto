#include "estructuras.h"


void manejadorDeHilos(){
	int socketCliente;

	// Funcion principal
	while((socketCliente = aceptarConexionDeCliente(socketListener))) { 	// hago el accept

		pthread_t thread_id;
    	pthread_attr_t attr;
    	pthread_attr_init(&attr);
    	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		//Creo hilo atendedor
		pthread_create( &thread_id , &attr, (void*) atenderNotificacion , (void*) &socketCliente);

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
			pthread_detach(pthread_self());
			break;

		case ES_PLANIFICADOR:{
			log_info(logger,"Se ha conectado el PLANIFICADOR");
			socketPlanificador = socket;
			infoHilos * datosHiloESI = (infoHilos*) malloc(sizeof(infoHilos));
			datosHiloESI->socket = socket;
			datosHiloESI->hiloAtendedor = pthread_self();
			pthread_mutex_lock(&mutexHilos);
			list_add(listaDeHilos, datosHiloESI);
			pthread_mutex_unlock(&mutexHilos);
			sendDeNotificacion(socket, ES_COORDINADOR);
		}

			break;

		case ES_ESI:{
			infoHilos * datosHiloESI = (infoHilos*) malloc(sizeof(infoHilos));
			datosHiloESI->socket = socket;
			datosHiloESI->hiloAtendedor = pthread_self();
			pthread_mutex_lock(&mutexHilos);
			list_add(listaDeHilos, datosHiloESI);
			pthread_mutex_unlock(&mutexHilos);

			manejarESI(socket);
		}
			break;

		case ES_INSTANCIA:{
			infoHilos * datosHiloESI = (infoHilos*) malloc(sizeof(infoHilos));
			datosHiloESI->socket = socket;
			datosHiloESI->hiloAtendedor = pthread_self();
			pthread_mutex_lock(&mutexHilos);
			list_add(listaDeHilos, datosHiloESI);
			pthread_mutex_unlock(&mutexHilos);
			manejarInstancia(socket);		//Mando sendDeNotificacion, tamaño de estructuras.
		}
			break;

		case 0:
			log_warning(logger, "El socket %d corto la conexion", socket);
			close(socket);
			pthread_detach(pthread_self());
			break;

		default:
			log_warning(logger, "La conexion recibida es erronea");
			close(socket);
			pthread_detach(pthread_self());
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
	 			manejarOperacionStore(socket);
	 			break;
	 		}

	 		case 0:{
				eliminarHiloDeConexion(socket);
				close(socket);
				break;
	 		}

	 		default:
				eliminarHiloDeConexion(socket);
				close(socket);
	 			break;


	 	}

	}

}

void manejarOperacionGet(int socket){

	log_trace(logger, "REALIZANDO OPERACION GET!");
	int id = buscarIdEsi(socket);

	char* claveNueva = recibirString(socket);
	log_info(logger, "Recibo clave %s por parte del ESI NRO = %d", claveNueva, id);


	log_warning(logger,"!!!!!!!!!!PROCESANDO INSTRUCCION!!!!!!!!!!!!!");
	sleep(RETARDO/1000);

	/*
	// Cacheo Error de tamaño de clave
	if(string_length(claveNueva) > TAMANIO_MAX_CLAVE){
		log_warning(logger, "Tamaño de la clave es mayor a 40 caracteres. Mandando a matar al ESI");
		sendDeNotificacion(socket,ERROR_TAMANIO_CLAVE); // SOCKET DEL ESI
	}*/

	int tamanio = 0;
	void * claveBloqueada = malloc(string_length(claveNueva) + sizeof(uint32_t)*2);
	int tamanioClave = string_length(claveNueva);
	memcpy(claveBloqueada + tamanio, &tamanioClave , sizeof(uint32_t));
	tamanio += sizeof(uint32_t);
	memcpy(claveBloqueada + tamanio, claveNueva, string_length(claveNueva));
	tamanio += string_length(claveNueva);
	memcpy(claveBloqueada + tamanio, &id, sizeof(uint32_t));
	tamanio += sizeof(uint32_t);

	sendRemasterizado(socketPlanificador,OPERACION_GET,tamanio,claveBloqueada);
	free(claveBloqueada);
	log_info(logger,"Replico CLAVE NUEVA a PLANIFICADOR");

	uint32_t respuesta = recvDeNotificacion(socketPlanificador);		// rta del planificador por si pudo guardar la clave.
	log_info(logger,"Recibo RESPUESTA por parte de PLANIFICADOR");

	//Catcheo la respuesta
	switch(respuesta){
		case OPERACION_EXITO:{
			log_debug(logger, "ASIGNO INSTANCIA PARA UN SET/STORE FUTURO.");
			infoAlgoritmoDistribucion * instancia = elegirInstancia(claveNueva);
			agregarClave(instancia->socketInstancia, claveNueva);
			log_debug(logger, "Actualizo mis estructuras administrativas...");

			//LOGUEAR RESPUESTA EN CASO DE SER EXITOSA
			loguearRespuestaGet(id,claveNueva);
			sendDeNotificacion(socket, respuesta);		// Le digo si pude realizar todo bien.
			log_info(logger,"Replico RESPUESTA a ESI NRO = %d", id);
			free(claveNueva);
		}
		break;

		case BLOQUEAR_ESI:{
			free(claveNueva);
			log_error(logger,"AVISANDO AL ESI = %d QUE SE VA A BLOQUEAR", id);
			sendDeNotificacion(socket,respuesta);
		}
		break;

		case 0:
			break;

		default:
			printf("a");
			break;


	}


}

void manejarOperacionSet(int socket){

	log_trace(logger, "REALIZANDO OPERACION SET!");
	int id = buscarIdEsi(socket);

	char * claveNueva = recibirString(socket);
	log_info(logger, "Recibo clave %s por parte del ESI NRO = %d", claveNueva, id);
	char * valorParaAlmacenar = recibirString(socket);
	log_info(logger, "Recibo valor %s por parte del ESI NRO = %d", valorParaAlmacenar, id);


	if(existeClave(claveNueva)){
		infoAlgoritmoDistribucion * instancia = encontrarInstanciaConClave(claveNueva);//elegirInstancia(claveNueva);
		int socketInstancia = instancia->socketInstancia;

		log_warning(logger,"!!!!!!!!!!PROCESANDO INSTRUCCION!!!!!!!!!!!!!");
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
		log_info(logger,"Replico CLAVE Y VALOR a INSTANCIA");

		int resultadoOperacionSet = recvDeNotificacion(socketInstancia);
		log_info(logger,"Recibo RESPUESTA de la INSTANCIA elegida.");

		if(resultadoOperacionSet == OPERACION_EXITO){
			sendDeNotificacion(socket, OPERACION_EXITO);
			log_trace(logger, "OPERACION SET realizada con EXITO!");
			log_info(logger, "Envio RESPUESTA de operacion a ESI NRO = %d", id);

			//LOGUEAR RESPUESTA EN CASO DE SER EXITOSA
			loguearRespuestaSet(id,claveNueva,valorParaAlmacenar);
			log_info(logger, "Logueando operacion en LOG DE OPERACIONES para el ESI NRO = %d", id);

			free(claveNueva);
			free(valorParaAlmacenar);

		} else{
			free(claveNueva);
			free(valorParaAlmacenar);
			log_error(logger,"Informando al ESI NRO = %d que no se pudo realizar la OPERACION SET", id);
			sendDeNotificacion(socket, ERROR_DE_INSTANCIA);
		}

	} else{
		free(claveNueva);
		free(valorParaAlmacenar);
		log_error(logger, "ERROR DE CLAVE NO IDENTIFICADA! ABORTANDO ESI NRO = %d", id);
		sendDeNotificacion(socket, ERROR_CLAVE_NO_IDENTIFICADA);
		close(socket);
	}

}


void manejarOperacionStore(int socket){

	log_trace(logger, "REALIZANDO OPERACION STORE!");
	int id = buscarIdEsi(socket);

	char* claveNueva = recibirString(socket);
	log_info(logger, "Recibo clave %s por parte del ESI NRO = %d", claveNueva, id);


	log_warning(logger,"!!!!!!!!!!PROCESANDO INSTRUCCION!!!!!!!!!!!!!");
	sleep(RETARDO/1000);


	int tamanio = 0;
	void * claveBloqueada = malloc(string_length(claveNueva) + sizeof(uint32_t));
	int tamanioClave = string_length(claveNueva);
	memcpy(claveBloqueada + tamanio, &tamanioClave , sizeof(uint32_t));
	tamanio += sizeof(uint32_t);
	memcpy(claveBloqueada + tamanio, claveNueva, string_length(claveNueva));
	tamanio += string_length(claveNueva);

	sendRemasterizado(socketPlanificador,OPERACION_STORE,tamanio,claveBloqueada);
	log_info(logger,"Replico CLAVE A PERSISTR a PLANIFICADOR");

	uint32_t respuesta = recvDeNotificacion(socketPlanificador);		// rta del planificador por si pudo guardar la clave.
	log_info(logger,"Recibo RESPUESTA por parte de PLANIFICADOR");

	//Catcheo la respuesta
	switch(respuesta){
		case OPERACION_EXITO:{
			log_debug(logger, "BUSCANDO INSTANCIA PARA PERSISTIR CLAVE");
			infoAlgoritmoDistribucion * instancia = encontrarInstanciaConClave(claveNueva);
			//Elimino de mi estructura administrativa de INSTANCIA elegida
			eliminarClave(instancia->socketInstancia, claveNueva);

			sendRemasterizado(instancia->socketInstancia,OPERACION_STORE,tamanio,claveBloqueada);
			free(claveBloqueada);
			log_debug(logger, "Actualizo mis estructuras administrativas...");

			uint32_t respuestaInstancia = recvDeNotificacion(instancia->socketInstancia);
			log_info(logger, "Recibo respuesta de la INSTANCIA elegida.");

			//LOGUEAR RESPUESTA EN CASO DE SER EXITOSA
			loguearRespuestaStore(id,claveNueva);
			sendDeNotificacion(socket, respuestaInstancia);		// Le digo si pude realizar todo bien.
			log_info(logger,"Replico RESPUESTA a ESI NRO = %d", id);
		}
		break;

		case ABORTAR_ESI:
			log_error(logger, "ABORTANDO EL ESI = %d AL NO PODER HACER EL STORE", id);
			sendDeNotificacion(socket, ABORTAR_ESI);
			free(claveBloqueada);
			//eliminarHiloDeConexion(socket);
			close(socket);
			break;

		case 0:
			free(claveBloqueada);
			break;

		default:
			free(claveBloqueada);
			break;
	}

	free(claveNueva);

}




//Me devuelve la estructura de la instancia encontrada a traves del algoritmo
infoAlgoritmoDistribucion*	elegirInstancia(char* claveNueva){

	infoAlgoritmoDistribucion * instancia;

	// Cacheo si la lista esta vacia
	if(list_is_empty(instanciasConectadas)){
		log_error(logger, "No hay instancias para atender la solicitud");
		return NULL;
	}

	if(strcmp(ALGORITMO, "LSU") == 0){
		instancia = getInstanciaLeastSpaceUsed();
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
	info->cantidadEntradasLibres = 0;

	pthread_mutex_lock(&instConectadas);
	list_add(instanciasConectadas,info);
	pthread_mutex_unlock(&instConectadas);


	log_info(logger,"Enlisto nueva INSTANCIA conectada");
}

void agregarESI(int socket, int ID_ESI){
	infoESI * info = (infoESI *) malloc(sizeof(infoESI));

	info->socket = socket;
	info->ID_ESI = ID_ESI;

	pthread_mutex_lock(&EsisConectados);
	list_add(ESIsConectados,info);
	pthread_mutex_unlock(&EsisConectados);

	log_info(logger, "Enlisto nuevo ESI conectado");
}

//Busco si la clave en alguna estructura de todas las instancias conectadas
bool existeClave(char* claveNueva){

	int i;
	int largoLista = list_size(instanciasConectadas);

	bool respuesta;

	for(i=0; i<largoLista; i++){

		infoAlgoritmoDistribucion * info = (infoAlgoritmoDistribucion*) list_get(instanciasConectadas, i);

		if((respuesta = existeClaveEnLista(info->clavesTomadas, claveNueva))){
			log_debug(logger,"EXISTE CLAVE EN LA LISTA DEL SOCKET NRO = %d", info->socketInstancia);
			break;
		}
	}

	return respuesta;


}

//Funcion aux para saber si existe clave
bool existeClaveEnLista(t_list * lista, char* clave){

	//Funcion aux para comparar claves ya tomadas
	bool buscarClave(char* claveNueva){
		return (strcmp(claveNueva, clave)==0);
	}

	return list_any_satisfy(lista, (void*) buscarClave);
}



int buscarIdEsi(int socket){

	bool buscarSocket(infoESI* info){
		return (info->socket == socket);
	}

	infoESI* data = list_find(ESIsConectados,(void*) buscarSocket);

	return data->ID_ESI;

}




//Busco en la lista de instancias conectadas el socket y guardo la clave en la lista del socket
void agregarClave(int socket, char* clave){

	bool buscarSocket(infoAlgoritmoDistribucion* info){
		return (info->socketInstancia == socket);
	}

	infoAlgoritmoDistribucion * data = list_find(instanciasConectadas, (void*) buscarSocket);
	char * palabra = string_new();
	string_append(&palabra, clave);

	pthread_mutex_lock(&clavesTomadas);
	list_add(data->clavesTomadas, palabra);
	pthread_mutex_unlock(&clavesTomadas);
}

void eliminarClave(int socketInstancia, char* claveAEliminar){

	//Funcion aux para encontrar socket
	bool buscarSocket(infoAlgoritmoDistribucion* info){
		return (info->socketInstancia == socketInstancia);
	}

	//Encuentro a traves del socket la info completa
	infoAlgoritmoDistribucion * data = list_find(instanciasConectadas, (void*) buscarSocket);

	//Funcion aux para comparar claves ya tomadas
	bool buscarClave(char* claveNueva){
		return (strcmp(claveNueva, claveAEliminar)==0);
	}

	pthread_mutex_lock(&clavesTomadas);
	void * clave = list_remove_by_condition(data->clavesTomadas, (void*) buscarClave);
	pthread_mutex_unlock(&clavesTomadas);

	free(clave);

}

infoAlgoritmoDistribucion * encontrarInstanciaConClave(char* claveNueva){

	int largo = list_size(instanciasConectadas);
	int i;

	infoAlgoritmoDistribucion * info;

	for(i=0; i < largo; i++){

		//Saco la info de cada posicion.
		info = (infoAlgoritmoDistribucion*) list_get(instanciasConectadas, i);

		//Busco la clave en la lista de la estructura recien encontrada.
		if(existeClaveEnLista(info->clavesTomadas, claveNueva)){
			break;
		}
	}

	return info;

}



