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
	char* file = "OPERACIONES_COORDINADOR.txt";
	operaciones = log_create(file, "OPERACIONES", 0, 0);	// log de operaciones
	log_info(operaciones, "CREO LOG DE OPERACIONES");

}

void liberarMemoriaCoordinador(){
	list_destroy_and_destroy_elements(instanciasConectadas,free);
	list_destroy_and_destroy_elements(ESIsConectados,free);
	log_destroy(operaciones);
	log_destroy(logger);
	exit(-1);
}
