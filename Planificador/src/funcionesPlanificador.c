#include "funcionesPlanificador.h"
#include "funcionesESI.h"

void cargarConfigPlanificador(t_config* configuracion){


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


	if (!config_has_property(configuracion, "ALFA_PLANIFICACION")) {
		printf("No se encuentra el parametro ALFA_PLANIFICACION en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro ALFA_PLANIFICACION en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	ALFA_PLANIFICACION = config_get_int_value(configuracion,"ALFA_PLANIFICACION");
	printf("ALFA_PLANIFICACION=%d\n", ALFA_PLANIFICACION);


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


void * atenderNotificacion(void * paqueteSocket){

	int socket = *(int*)paqueteSocket;
	uint32_t nroNotificacion = recvDeNotificacion(socket);

	switch(nroNotificacion){

		case ES_COORDINADOR:
			log_warning(logger, "La conexion recibida es erronea");
			//FD_CLR(socket, &socketClientes);
			close(socket);
			pthread_detach(pthread_self());
			break;

		case ES_PLANIFICADOR:
			log_warning(logger, "La conexion recibida es erronea");
			//FD_CLR(socket, &socketClientes);
			close(socket);
			pthread_detach(pthread_self());
			break;

		case ES_ESI:
			log_info(logger,"Se ha conectado un ESI");
			sendDeNotificacion(socket, ES_PLANIFICADOR);

			infoHilos * datosHilo = (infoHilos*) malloc(sizeof(infoHilos));
			datosHilo->socket = socket;
			datosHilo->hiloAtendedor = pthread_self();

			pthread_mutex_lock(&mutexHilos);
			list_add(hilosParaConexiones, datosHilo);
			pthread_mutex_unlock(&mutexHilos);

			manejarESI(socket);
			break;

		case ES_INSTANCIA:
			log_warning(logger, "La conexion recibida es erronea");
			//FD_CLR(socket, &socketClientes);
			close(socket);
			pthread_detach(pthread_self());
			break;

		case 0:
			log_warning(logger, "El socket %d corto la conexion", socket);
			//FD_CLR(socket, &socketClientes);
			close(socket);
			pthread_detach(pthread_self());
			break;

		default:
			log_warning(logger, "La conexion recibida es erronea");
			//FD_CLR(socket, &socketClientes);
			close(socket);
			pthread_detach(pthread_self());
			break;

	}

	return 0;
}

void atenderConexion(int socketNuevo){
	int socketAceptado = aceptarConexionDeCliente(socketNuevo);
	FD_SET(socketAceptado, &socketClientes);
	socketMaximo = calcularSocketMaximo(socketAceptado,socketMaximo);
	log_info(logger,"Se ha recibido una nueva conexion!");
}


void* manejarCoordinador(){

	log_debug(logger, "Escuchando indicaciones del COORDINADOR!");
	int corte = 1;
	while(corte){

		uint32_t notificacion = recvDeNotificacion(socketServerCoordinador);

		switch(notificacion){

		case OPERACION_GET:{
			char* clave = recibirString(socketServerCoordinador);
			log_trace(logger,"RECIBO CLAVE NUEVA = %s POR PARTE DEL COORDINADOR", clave);
			int id = recibirUint(socketServerCoordinador);
			log_info(logger, "INTENTO BLOQUEAR LA CLAVE = %s", clave);
			int resultadoOperacion = agregarClave(clave,id);
			manejarRespuestaAgregarClave(resultadoOperacion);
			free(clave);
			}
			break;

		case OPERACION_SET:{
			log_error(logger,"NO ME DEBERIA LLEGAR UNA OPERACION SET");
			log_error(logger,"MURIENDO CON ESTILO!");
			liberarMemoriaPlanificador();
			}
			break;

		case OPERACION_STORE:{
			char* clave = recibirString(socketServerCoordinador);
			log_trace(logger,"RECIBO CLAVE A PERSISTIR = %s POR PARTE DEL COORDINADOR", clave);
			log_info(logger, "INTENTO DESBLOQUEAR LA CLAVE = %s", clave);
			int resultadoOperacion = eliminarClave(clave);
			manejarRespuestaEliminarClave(resultadoOperacion);
			free(clave);
			}
			break;

		case 0:
			liberarMemoriaPlanificador();
			pthread_detach(pthread_self());
			break;

		default:
			pthread_detach(pthread_self());
			break;

		}

	}

	return 0;

}

void manejarRespuestaAgregarClave(int resultado){

	switch(resultado){
		case BLOQUEAR_ESI:
			log_error(logger, "Avisando a COORDINADOR que voy a bloquear AL ESI: %d", ESI_EJECUTANDO->ID_ESI);
			sendDeNotificacion(socketServerCoordinador, BLOQUEAR_ESI);
			break;

		case OPERACION_EXITO:
			log_info(logger,"Avisando a COORDINADOR que salio todo bien..");
			sendDeNotificacion(socketServerCoordinador, OPERACION_EXITO);
			break;

		default:
			log_error(logger, "ERROR AL INTENTAR METER CLAVE EN LISTA DE CLAVES BLOQUEADAS");
		}
}

void manejarRespuestaEliminarClave(int resultado){

	switch(resultado){
		case OPERACION_EXITO:
			log_info(logger,"Avisando a COORDINADOR que salio todo bien..");
			sendDeNotificacion(socketServerCoordinador, OPERACION_EXITO);
			break;

		case ABORTAR_ESI:
			log_error(logger, "Avisando a COORDINADOR que no pude eliminar la clave");
			sendDeNotificacion(socketServerCoordinador, ABORTAR_ESI);
			break;

		default:
			log_error(logger,"ERROR AL INTENTAR ELIMINAR CLAVE EN LISTA DE CLAVES BLOQUEADAS");

	}

}

int agregarClave(char* clave, int id){

	int resultadoOperacion;

	if(existeClaveEnLista(clave)){
		pasarESIABloqueado(clave,id);			// ACA YA BLOQUEO AL ESI ACTUAL QUE ESTABA EJECUTANDO
		resultadoOperacion = BLOQUEAR_ESI;
	} else{
		infoClaveBloqueada * info = malloc(sizeof(infoClaveBloqueada));

		info->claveBloqueada = string_new();
		string_append(&info->claveBloqueada, clave);
		info->ID_ESI = id;

		pthread_mutex_lock(&clavesBloqueadas);
		list_add(listaClavesBloqueadas, info);
		pthread_mutex_unlock(&clavesBloqueadas);
		//free(clave);
		resultadoOperacion = OPERACION_EXITO;
	}

	return resultadoOperacion;
}


int eliminarClave(char* claveAEliminar){
	int resultadoOperacion;
	if(existeClaveEnLista(claveAEliminar)){
		log_trace(logger, "EXISTE LA CLAVE = %s EN LISTA DE CLAVES BLOQUEADAS", claveAEliminar);
		desbloquearClave(claveAEliminar);
		resultadoOperacion = OPERACION_EXITO;
	} else{
		resultadoOperacion = ABORTAR_ESI;
	}

	return resultadoOperacion;
}

void eliminarClaveBloqueada(char* clave){

	int largo = list_size(listaClavesBloqueadas);
	int i;

	for(i=0; i<largo;i++){

		infoClaveBloqueada * info = (infoClaveBloqueada*) list_get(listaClavesBloqueadas,i);

		if(info != NULL){
			if((strcmp(info->claveBloqueada, clave)==0)){
				log_info(logger, "Eliminando %s de CLAVES BLOQUEADAS", clave);
				pthread_mutex_lock(&clavesBloqueadas);
				list_remove_and_destroy_element(listaClavesBloqueadas,i,(void*) liberarClave);
				pthread_mutex_unlock(&clavesBloqueadas);
				break;
			}
		}
	}
}

infoESI * eliminarESIBloqueado(infoESI * ESI){
	bool existeClaveEnColaDeBloqueados(void * info){
		infoESIBloqueado * data = (infoESIBloqueado*) info;
		return data->ESI->ID_ESI == ESI->ID_ESI;
	}

	pthread_mutex_lock(&colaBloqueados);
	infoESIBloqueado * info = (infoESIBloqueado*)list_remove_by_condition(listaBloqueados, (void*) existeClaveEnColaDeBloqueados);
	pthread_mutex_unlock(&colaBloqueados);
	return info->ESI;
}

void desbloquearClave(char* claveAEliminar){
	// AL ELIMINAR LA CLAVE ME FIJO SI PUEDO DESBLOQUEAR ALGUN ESI QUE FUE BLOQUEADO POR ESA CLAVE!!
	if(existeESIBloqueadoPorClave(claveAEliminar)){
		infoESI * ESI_PARA_LISTOS = encontrarESIConClaveBloqueada(claveAEliminar);
		if(ESI_PARA_LISTOS == NULL){
			log_error(logger, "NO PUDE ENCONTRAR EL ESI CON CLAVE = %s", claveAEliminar);
		} else{

			log_trace(logger, "EXISTE ESI = %d BLOQUEADO CON CLAVE = %s", ESI_PARA_LISTOS->ID_ESI, claveAEliminar);
			eliminarClaveBloqueada(claveAEliminar);
			log_trace(logger,"ELIMINANDO ESI = %d CON CLAVE BLOQUEADA = %s DE COLA BLOQUEADOS", ESI_PARA_LISTOS->ID_ESI, claveAEliminar);

			/*************************/
			//PASARLE EL ESI ENCONTRADO Y QUE ME DEVUELVA LA MISMA ESTRUCTURA PERO YA ELIMINADA DE LA COLA DE BLOQUEADOS
			infoESI * ESI = eliminarESIBloqueado(ESI_PARA_LISTOS);
			/************************/

			log_trace(logger, "ESI = %d PASANDOLO A LA COLA DE LISTOS CON SOCKET = %d", ESI->ID_ESI, ESI->socketESI);
			//ESI_PARA_LISTOS->estado = listo;
			pthread_mutex_lock(&colaReady);
			list_add(listaListos, ESI);
			pthread_mutex_unlock(&colaReady);
			sem_post(&esiListos);
			log_trace(logger, "Encolando ESI = %d -> COLA DE READY", ESI->ID_ESI);
		}

	} else{
		log_warning(logger, "NO EXISTE ESI BLOQUEADO CON CLAVE = %s", claveAEliminar);
		eliminarClaveBloqueada(claveAEliminar);
	}
}

// Me fijo si la clave esta en la lista de claves causantes por el bloqueo de un ESI
bool existeESIBloqueadoPorClave(char* clave){

	bool existeClaveEnColaDeBloqueados(infoESIBloqueado * data){
		return (strcmp(data->claveCausante, clave)==0);
	}

	return list_any_satisfy(listaBloqueados, (void*) existeClaveEnColaDeBloqueados);
}

//Encuentro la informacion del ESI por la clave por la cual fue bloqueado
infoESI * encontrarESIConClaveBloqueada(char * clave){

	bool existeClaveEnColaDeBloqueados(void * info){
		infoESIBloqueado * data = (infoESIBloqueado*) info;
		return (strcmp(data->claveCausante, clave)==0);
	}

	infoESIBloqueado * datos = (infoESIBloqueado*) list_find(listaBloqueados,(void*) existeClaveEnColaDeBloqueados);

	return datos->ESI;

}

//Verifico si existe clave en lista de claves bloqueadas
bool existeClaveEnLista(char* clave){
	//Funcion aux para comparar claves ya tomadas
	bool buscarClave(void * info){
		infoClaveBloqueada * data = (infoClaveBloqueada *) info;
		return (strcmp(data->claveBloqueada, clave)==0);
	}
	return list_any_satisfy(listaClavesBloqueadas, (void*) buscarClave);
}

void chequeoSocket(int socket){
	if(socket < 0){
		log_error(logger, "Fallo accept de Coordinador");
		perror("Fallo accept");
		exit(-1);
	}
}


void manejarConexiones(){
	//Creo servidor para los ESI
	socketListener = iniciarServidor(PUERTO_ESCUCHA);

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





	/*
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
			liberarMemoriaPlanificador();
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
	}*/
}

void iniciarHiloConsola(){
	pthread_attr_t attr1;
	pthread_attr_init(&attr1);
	pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);
	pthread_create(&hiloConsola , &attr1,(void*) levantarConsolaPlanificador,NULL);

	infoHilos * datosHilo = (infoHilos*) malloc(sizeof(infoHilos));
	datosHilo->socket = 0;
	datosHilo->hiloAtendedor = hiloConsola;

	pthread_mutex_lock(&mutexHilos);
	list_add(hilosParaConexiones, datosHilo);
	pthread_mutex_unlock(&mutexHilos);
}

void iniciarHiloQueAtiendeAlCoordinador(){
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&hiloCoordinador, &attr , (void*) manejarCoordinador, NULL);

	infoHilos * datosHilo = (infoHilos*) malloc(sizeof(infoHilos));
	datosHilo->socket = 0;
	datosHilo->hiloAtendedor = hiloCoordinador;

	pthread_mutex_lock(&mutexHilos);
	list_add(hilosParaConexiones, datosHilo);
	pthread_mutex_unlock(&mutexHilos);
}

void iniciarHiloQuePlanifica(){
	pthread_attr_t atr;
	pthread_attr_init(&atr);
	pthread_attr_setdetachstate(&atr, PTHREAD_CREATE_DETACHED);
	pthread_create(&hiloPlanificador,&atr, (void*) planificar, NULL);

	infoHilos * datosHilo = (infoHilos*) malloc(sizeof(infoHilos));
	datosHilo->socket = 0;
	datosHilo->hiloAtendedor = hiloPlanificador;

	pthread_mutex_lock(&mutexHilos);
	list_add(hilosParaConexiones, datosHilo);
	pthread_mutex_unlock(&mutexHilos);
}



void iniciarEstructurasAdministrativasPlanificador(){
	log_info(logger, "GENERANDO ESTRUCTURAS ADMINISTRATIVAS!");
	posicionListaListos = 0;

	hilosParaConexiones = list_create();
	listaListos = list_create();
	listaBloqueados = list_create();
	listaTerminados = list_create();
	listaClavesBloqueadas = list_create();

	//ESI_EJECUTANDO = (infoESI*) malloc(sizeof(infoESI));

	//Inicializo los semaforos

	sem_init(&sistemaEnEjecucion, 0, 1);
	sem_init(&semPlanificar, 0, 1);
	sem_init(&esiListos, 0, 0);

	pthread_mutex_init(&mutexHilos,NULL);
	pthread_mutex_init(&ESI_EXECUTOR,NULL);
	pthread_mutex_init(&colaReady, NULL);
	pthread_mutex_init(&colaBloqueados, NULL);
	pthread_mutex_init(&clavesBloqueadas, NULL);
	pthread_mutex_init(&colaFinalizados,NULL);

	// Agrego todas las claves bloqueadas de archivo de configuracion a las claves bloqueadas del sistema
	int cantidadClavesBloqueadas = 0;
	while(CLAVES_BLOQUEADAS[cantidadClavesBloqueadas] != NULL){
		infoClaveBloqueada * info = malloc(sizeof(infoClaveBloqueada));
		info->claveBloqueada = CLAVES_BLOQUEADAS[cantidadClavesBloqueadas];
		info->ID_ESI = 0;

		log_info(logger, "AGREGANDO CLAVE BLOQUEADA = %s A LA LISTA DE CLAVES BLOQUEADAS.", info->claveBloqueada);

		pthread_mutex_lock(&clavesBloqueadas);
		list_add(listaClavesBloqueadas, info);
		pthread_mutex_unlock(&clavesBloqueadas);
		cantidadClavesBloqueadas++;
	}
}

void cerrandoSockets(t_list * lista){

	int largo = list_size(lista);
	int i;

	for(i=0; i<largo ; i++){

		infoESI * data = (infoESI*) list_get(lista, i);

		close(data->socketESI);
	}
}


void liberarESI(infoESI* ESI){
	close(ESI->socketESI);
	free(ESI);
}

void liberarESISDeBloqueados(void * info){

	infoESIBloqueado * data = (infoESIBloqueado*) info;
	close(data->ESI->socketESI);
	free(data->claveCausante);
	//free(data->ESI);
	free(data);
}

void liberarClave(infoClaveBloqueada* info){
	free(info->claveBloqueada);
	free(info);
}

void liberarHilo(infoHilos * data){
	close(data->socket);
	pthread_detach(data->hiloAtendedor);
	free(data);
}

void parcaDePlanificador(int signal){
	log_error(logger, "MURIENDO CON ELEGANCIA...");
	//log_trace(logger, "CERRANDO SOCKETS");
	// CERRAR LOS SOCKETS DE CADA LISTA
	//cerrandoSockets(listaListos);
	//cerrandoSockets(listaBloqueados);
	//cerrandoSockets(listaTerminados);
	//close(socketServerCoordinador);
	//close(socketListener);
	log_trace(logger, "DESTRUYENDO ESTRUCTURAS ADMINISTRATIVAS");
	// DESTRUYO LISTAS Y SUS ELEMENTOS
	list_destroy_and_destroy_elements(listaListos,(void*) liberarESI);
	list_destroy_and_destroy_elements(listaBloqueados,(void*) liberarESISDeBloqueados);
	list_destroy_and_destroy_elements(listaTerminados,(void*) liberarESI);
	list_destroy_and_destroy_elements(listaClavesBloqueadas,(void*) liberarClave);
	list_destroy_and_destroy_elements(hilosParaConexiones, (void*) liberarHilo);
	pthread_mutex_destroy(&mutexHilos);
	pthread_mutex_destroy(&colaReady);
	pthread_mutex_destroy(&colaBloqueados);
	pthread_mutex_destroy(&clavesBloqueadas);
	pthread_mutex_destroy(&ESI_EXECUTOR);
	pthread_mutex_destroy(&colaFinalizados);
	sem_destroy(&sistemaEnEjecucion);
	sem_destroy(&semPlanificar);
	sem_destroy(&esiListos);
	pthread_detach(hiloConsola);
	pthread_detach(hiloCoordinador);
	pthread_detach(hiloPlanificador);
	log_warning(logger, "CERRANDO PLANIFICADOR");
	log_destroy(logger);
	exit(-1);
}


void liberarMemoriaPlanificador(){
	log_error(logger, "MURIENDO CON ELEGANCIA...");
	log_trace(logger, "CERRANDO SOCKETS");
	// CERRAR LOS SOCKETS DE CADA LISTA
	//cerrandoSockets(listaListos);
	//cerrandoSockets(listaBloqueados);
	//cerrandoSockets(listaTerminados);
	//close(socketServerCoordinador);
	close(socketListener);
	log_trace(logger, "DESTRUYENDO ESTRUCTURAS ADMINISTRATIVAS");
	// DESTRUYO LISTAS Y SUS ELEMENTOS
	list_destroy_and_destroy_elements(listaListos,(void*) liberarESI);
	list_destroy_and_destroy_elements(listaBloqueados,(void*) liberarESI);
	list_destroy_and_destroy_elements(listaTerminados,(void*) liberarESI);
	list_destroy_and_destroy_elements(listaClavesBloqueadas,(void*) liberarClave);
	list_destroy_and_destroy_elements(hilosParaConexiones, (void*) liberarHilo);
	pthread_mutex_destroy(&mutexHilos);
	pthread_mutex_destroy(&colaBloqueados);
	pthread_mutex_destroy(&colaReady);
	pthread_mutex_destroy(&clavesBloqueadas);
	pthread_mutex_destroy(&ESI_EXECUTOR);
	pthread_mutex_destroy(&colaFinalizados);
	sem_destroy(&sistemaEnEjecucion);
	sem_destroy(&semPlanificar);
	sem_destroy(&esiListos);
	pthread_detach(hiloConsola);
	pthread_detach(hiloCoordinador);
	pthread_detach(hiloPlanificador);
	log_warning(logger, "CERRANDO PLANIFICADOR");
	log_destroy(logger);
	exit(-1);
}
