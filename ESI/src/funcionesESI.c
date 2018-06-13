#include "funcionesESI.h"

FILE * abrirArchivoAParsear(char * nombreArchivo){
	  FILE * archivo = fopen(nombreArchivo, "r");
	    if (archivo == NULL){
	    	//log_error(No pude abrir el archivo);
	        perror("Error al abrir el archivo: ");
	        exit(-1);
	    }
	    return archivo;
}

// @modo EJEMPLO
void parsearArchivo(FILE * archivo){
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, archivo)) != -1) {
        t_esi_operacion parsed = parse(line);

        if(parsed.valido){
            switch(parsed.keyword){
                case GET:
                    printf("GET\tclave: <%s>\n", parsed.argumentos.GET.clave);
                    break;
                case SET:
                    printf("SET\tclave: <%s>\tvalor: <%s>\n", parsed.argumentos.SET.clave, parsed.argumentos.SET.valor);
                    break;
                case STORE:
                    printf("STORE\tclave: <%s>\n", parsed.argumentos.STORE.clave);
                    break;
                default:
                    fprintf(stderr, "No pude interpretar <%s>\n", line);
                    exit(EXIT_FAILURE);
            }

            destruir_operacion(parsed);
        } else {
            fprintf(stderr, "La linea <%s> no es valida\n", line);
            exit(EXIT_FAILURE);
        }
    }

    fclose(archivo);
    if (line)
        free(line);

}


void cargarConfigESI(t_config* configuracion){

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

	if (!config_has_property(configuracion, "PLANIFICADOR_IP")) {
		printf("No se encuentra el parametro PLANIFICADOR_IP en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro PLANIFICADOR_IP en el archivo de configuracion");
		config_destroy(configuracion);
		//liberarMemoria();
		exit(-1);
	}

	PLANIFICADOR_IP = string_new();
	string_append(&PLANIFICADOR_IP, config_get_string_value(configuracion, "PLANIFICADOR_IP"));
	printf("PLANIFICADOR_IP=%s\n", PLANIFICADOR_IP);

	if (!config_has_property(configuracion, "PUERTO_PLANIFICADOR")) {
		printf("No se encuentra el parametro PUERTO_PLANIFICADOR en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro PUERTO_PLANIFICADOR en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	PUERTO_PLANIFICADOR = config_get_int_value(configuracion,"PUERTO_PLANIFICADOR");
	printf("PUERTO_PLANIFICADOR=%d\n", PUERTO_PLANIFICADOR);



	config_destroy(configuracion);

}

/*
int tamanio = 0;
void * claveBloqueada = malloc(string_length(claveNueva) + sizeof(uint32_t));
int tamanioClave = string_length(claveNueva);
memcpy(claveBloqueada + tamanio, &tamanioClave , sizeof(uint32_t));
tamanio += sizeof(uint32_t);
memcpy(claveBloqueada + tamanio, claveNueva, string_length(claveNueva));
tamanio += string_length(claveNueva);

sendRemasterizado(socketPlanificador,OPERACION_GET,tamanio,claveBloqueada);
free(claveBloqueada);
*/

void manejarOperacionDeParseo(){

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    read = getline(&line, &len,archivoAParsear);
    t_esi_operacion parsed = parse(line);

    if(parsed.valido){
        switch(parsed.keyword){
            case GET:
            	log_info(logger, "Realizo operacion GET");
            	log_info(logger,"GET\tclave: <%s>\n", parsed.argumentos.GET.clave);

            	char * clave = string_new();
            	string_append(&clave,parsed.argumentos.GET.clave);

            	//Serializo la clave a enviar.
            	int tamanio = 0;
            	void * claveBloqueada = malloc(string_length(clave) + sizeof(uint32_t));
            	int tamanioClave = string_length(clave);
            	memcpy(claveBloqueada + tamanio, &tamanioClave, sizeof(uint32_t));
            	tamanio += sizeof(uint32_t);
            	memcpy(claveBloqueada + tamanio, clave, string_length(clave));
            	tamanio += string_length(clave);

            	sendRemasterizado(socketServerCoordinador, OPERACION_GET,tamanio,claveBloqueada);
            	log_info(logger,"Envio clave de la operacion GET a COORDINADOR");
            	free(claveBloqueada);

            	int respuesta = recvDeNotificacion(socketServerCoordinador);
            	log_info(logger,"Recibo respuesta por parte del COORDINADOR de la operacion GET");

            	sendDeNotificacion(socketServerPlanificador, respuesta);
            	log_info(logger,"Envio respuesta de la operacion GET a PLANIFICADOR");



                break;
            case SET:
            	log_info(logger, "Realizo operacion SET");
                log_info(logger,"SET\tclave: <%s>\tvalor: <%s>\n", parsed.argumentos.SET.clave, parsed.argumentos.SET.valor);
                break;
            case STORE:
            	log_info(logger, "Realizo operacion STORE");
            	log_info(logger,"STORE\tclave: <%s>\n", parsed.argumentos.STORE.clave);
                break;
            default:
                log_error(logger, "No pude interpretar <%s>\n", line);
                exit(EXIT_FAILURE);
        }

        destruir_operacion(parsed);
    } else {
        fprintf(stderr, "La linea <%s> no es valida\n", line);
        exit(EXIT_FAILURE);
    }

}


















