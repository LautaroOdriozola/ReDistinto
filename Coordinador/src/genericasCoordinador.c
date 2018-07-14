#include "genericasCoordinador.h"

void chequeoSocket(int socket){
	if(socket < 0){
		log_error(logger, "Fallo accept de Coordinador");
		perror("Fallo accept");
		exit(-1);
	}
}

void cargarConfigCoordinador(t_config* configuracion){

	if (!config_has_property(configuracion, "PUERTO_ESCUCHA")) {
		printf("No se encuentra el parametro PUERTO_ESCUCHA en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro PUERTO_ESCUCHA en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	PUERTO_ESCUCHA = config_get_int_value(configuracion,"PUERTO_ESCUCHA");
	printf("PUERTO_ESCUCHA=%d\n", PUERTO_ESCUCHA);


	if (!config_has_property(configuracion, "ALGORITMO")) {
		printf("No se encuentra el parametro ALGORITMO en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro ALGORITMO en el archivo de configuracion");
		config_destroy(configuracion);
		//liberarMemoria();
		exit(-1);
	}

	ALGORITMO = string_new();
	string_append(&ALGORITMO, config_get_string_value(configuracion, "ALGORITMO"));
	printf("ALGORITMO=%s\n", ALGORITMO);

	if (!config_has_property(configuracion, "CANTIDAD_ENTRADAS")) {
		printf("No se encuentra el parametro CANTIDAD_ENTRADAS en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro CANTIDAD_ENTRADAS en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	CANTIDAD_ENTRADAS = config_get_int_value(configuracion,"CANTIDAD_ENTRADAS");
	printf("CANTIDAD_ENTRADAS=%d\n", CANTIDAD_ENTRADAS);

	if (!config_has_property(configuracion, "TAMANIO_ENTRADA")) {
		printf("No se encuentra el parametro TAMANIO_ENTRADA en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro TAMANIO_ENTRADA en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	TAMANIO_ENTRADA = config_get_int_value(configuracion,"TAMANIO_ENTRADA");
	printf("TAMANIO_ENTRADA=%d\n", TAMANIO_ENTRADA);

	if (!config_has_property(configuracion, "RETARDO")) {
		printf("No se encuentra el parametro RETARDO en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro RETARDO en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	RETARDO = config_get_int_value(configuracion,"RETARDO");
	printf("RETARDO=%d\n", RETARDO);


	config_destroy(configuracion);

}


void iniciarEstructurasAdministrativasCoordinador(){
	log_info(logger, "GENERANDO ESTRUCTURAS ADMINISTRATIVAS");
	instanciasConectadas = list_create();
	ESIsConectados = list_create();
	listaDeHilos = list_create();

	pthread_mutex_init(&mutexHilos, NULL);
	pthread_mutex_init(&instConectadas, NULL);
	pthread_mutex_init(&EsisConectados, NULL);
	pthread_mutex_init(&clavesTomadas, NULL);


	char* file = "OPERACIONES_COORDINADOR.txt";
	operaciones = log_create(file, "OPERACIONES", 0, 0);	// log de operaciones
	log_info(operaciones, "CREO LOG DE OPERACIONES");

}

void loguearRespuestaGet(int id, char * claveNueva){
	log_info(operaciones, "ESI %d -> GET \t %s", id, claveNueva);
}

void loguearRespuestaSet(int id,char* claveNueva,char* valor){
	log_info(operaciones, "ESI %d -> SET \t %s \t %s", id, claveNueva, valor);
}

void loguearRespuestaStore(int id,char* claveNueva){
	log_info(operaciones, "ESI %d -> STORE \t %s", id, claveNueva);
}


void cerrandoSocketsInstancias(){

	int largo = list_size(instanciasConectadas);
	int i;

	for(i=0; i<largo ; i++){

		infoAlgoritmoDistribucion * data = (infoAlgoritmoDistribucion*) list_get(instanciasConectadas, i);

		close(data->socketInstancia);
	}
}

void cerrandoSocketsESIS(){

	int largo = list_size(ESIsConectados);
	int i;

	for(i=0; i<largo ; i++){

		infoESI * data = (infoESI*) list_get(ESIsConectados, i);

		close(data->socket);
	}
}

void liberarClavesDeInstancias(){

	int largo = list_size(instanciasConectadas);
	int i;

	for(i=0; i<largo; i++){

		infoAlgoritmoDistribucion * data = (infoAlgoritmoDistribucion *) list_get(instanciasConectadas,i);

		list_destroy_and_destroy_elements(data->clavesTomadas, free);
	}

}

void eliminarHiloDeConexion(int socketESI){

	bool encontrarSocket(infoHilos * info){
		return info->socket == socketESI;
	}

	pthread_mutex_lock(&mutexHilos);
	infoHilos * data = (infoHilos*) list_remove_by_condition(listaDeHilos,(void*) encontrarSocket);
	pthread_mutex_unlock(&mutexHilos);

	if(data == NULL){
		log_error(logger, "No se encontro el hilo de conexion que atendia al socket %d", socketESI);
	} else{
		log_warning(logger, "Eliminando hilo con conexion en socket = %d", socketESI);
		log_warning(logger, "Socket cerro la conexion!");

		pthread_cancel(data->hiloAtendedor);
		free(data);
	}
}



void liberarInstancias(infoAlgoritmoDistribucion* instancia){
	if(instancia != NULL){
		liberarClaves(instancia->clavesTomadas);
		free(instancia);
	}
}

void liberarClaves(t_list * lista){

	void destruirString(char* string) {
		free(string);
	}
	list_destroy_and_destroy_elements(lista, (void*) destruirString);
}

void liberarESIs(infoESI* ESI){
	if(ESI != NULL){
		free(ESI);
	}
}

void liberarHilo(infoHilos * data){
	if(data != NULL){
		close(data->socket);
		pthread_cancel(data->hiloAtendedor);
		free(data);
	}
}

void laParca(int signal){
	log_error(logger, "MURIENDO CON ELEGANCIA...");
	log_trace(logger, "DESTRUYENDO ESTRUCTURAS ADMINISTRATIVAS");
	list_destroy_and_destroy_elements(instanciasConectadas,(void*) liberarInstancias);
	list_destroy_and_destroy_elements(ESIsConectados,(void*) liberarESIs);
	list_destroy(listaDeHilos);
	pthread_mutex_destroy(&instConectadas);
	pthread_mutex_destroy(&EsisConectados);
	pthread_mutex_destroy(&clavesTomadas);
	log_warning(logger, "CERRANDO COORDINADOR");
	log_destroy(operaciones);
	log_destroy(logger);
	exit(-1);
}


void liberarMemoriaCoordinador(){
	log_error(logger, "MURIENDO CON ELEGANCIA...");
	log_trace(logger, "DESTRUYENDO ESTRUCTURAS ADMINISTRATIVAS");
	list_destroy_and_destroy_elements(instanciasConectadas,(void*) liberarInstancias);
	list_destroy_and_destroy_elements(ESIsConectados,(void*) liberarESIs);
	list_destroy(listaDeHilos);
	pthread_mutex_destroy(&instConectadas);
	pthread_mutex_destroy(&EsisConectados);
	pthread_mutex_destroy(&clavesTomadas);
	log_warning(logger, "CERRANDO COORDINADOR");
	log_destroy(operaciones);
	log_destroy(logger);
	exit(-1);
}
