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
/*
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

}*/


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

void manejarOperacionDeParseo(){

    char * line = NULL;
    size_t len = 0;
    //ssize_t read;

    //read = getline(&line, &len,archivoAParsear);
    getline(&line, &len,archivoAParsear);
    t_esi_operacion parsed = parse(line);

    if(parsed.valido){
        switch(parsed.keyword){

        	case GET:
            	log_info(logger, "Realizo operacion GET");
            	log_info(logger,"GET\tclave: <%s>\n", parsed.argumentos.GET.clave);

            	char * claveGet = string_new();
            	string_append(&claveGet,parsed.argumentos.GET.clave);

            	//Serializo la clave a enviar.
            	int tamanioGet = 0;
            	void * claveBloqueadaGet = malloc(string_length(claveGet) + sizeof(uint32_t));
            	int tamanioClaveGet = string_length(claveGet);
            	memcpy(claveBloqueadaGet + tamanioGet, &tamanioClaveGet, sizeof(uint32_t));
            	tamanioGet += sizeof(uint32_t);
            	memcpy(claveBloqueadaGet + tamanioGet, claveGet, string_length(claveGet));
            	tamanioGet += string_length(claveGet);

            	sendRemasterizado(socketServerCoordinador, OPERACION_GET,tamanioGet,claveBloqueadaGet);
            	log_info(logger,"Envio clave de la operacion GET a COORDINADOR");
            	free(claveBloqueadaGet);

            	log_info(logger,"Recibo respuesta por parte del COORDINADOR de la operacion GET");
            	int respuestaGet = recvDeNotificacion(socketServerCoordinador);

            	// Catcheo tipo de respuestas
            	switch(respuestaGet){

            		case ERROR_TAMANIO_CLAVE:
            			//matarEsi();
            			break;

            		case 0:
            			break;

            		default:
            			break; //Si no es ningun error, sigo con el flujo normal?

            	}


            	//sendDeNotificacion(socketServerPlanificador, respuestaGet);
            	log_info(logger,"Envio respuesta de la operacion GET a PLANIFICADOR");
                break;

            case SET:
              	log_info(logger, "Realizo operacion SET");
              	log_info(logger,"SET\tclave: <%s>\tvalor: <%s>\n", parsed.argumentos.SET.clave, parsed.argumentos.SET.valor);

              	char* claveSet = string_new();
              	string_append(&claveSet,parsed.argumentos.SET.clave);
              	char* valor = string_new();
              	string_append(&valor,parsed.argumentos.SET.valor);

              	//serializacion
              	int tamanioSet = 0;
              	void * claveBloqueadaSet = malloc(string_length(claveSet) +  string_length(valor)+ 2* sizeof(uint32_t));
              	int tamanioClaveSet = string_length(claveSet);
              	int tamanioValor = string_length(valor);
              	memcpy(claveBloqueadaSet+tamanioSet,&tamanioClaveSet, sizeof(uint32_t));
              	tamanioSet += sizeof(uint32_t);
              	memcpy(claveBloqueadaSet+tamanioSet,claveSet,string_length(claveSet));
              	tamanioSet+=string_length(claveSet);
              	memcpy(claveBloqueadaSet+tamanioSet,&tamanioValor,sizeof(uint32_t));
              	tamanioSet+= sizeof(uint32_t);
              	memcpy(claveBloqueadaSet+tamanioSet,valor,string_length(valor));
              	tamanioSet += string_length(valor);

              	sendRemasterizado(socketServerCoordinador,OPERACION_SET,tamanioSet,claveBloqueadaSet);
              	log_info(logger,"Envio clave y valor de la Operacion SET al coordinador");
              	free(claveBloqueadaSet);

              	int respuestaSet = recvDeNotificacion(socketServerCoordinador);
              	log_info(logger,"Recibo respuesta por parte del COORDINADOR de la operacion SET");

              	//sendDeNotificacion(socketServerPlanificador, respuestaSet);
              	log_info(logger,"Envio respuesta de la operacion SET a PLANIFICADOR");

              	break;

            case STORE:

            	log_info(logger, "Realizo operacion STORE");
            	log_info(logger,"STORE\tclave: <%s>\n", parsed.argumentos.STORE.clave);
            	char* claveStore = string_new();
            	string_append(&claveStore,parsed.argumentos.STORE.clave);

            	//serializacion
            	int tamanio = 0;
            	void * claveBloqueada = malloc(string_length(claveStore) + sizeof(uint32_t));
            	int tamanioClave = string_length(claveStore);
            	memcpy(claveBloqueada+tamanio,&tamanioClave,sizeof(uint32_t));
            	tamanio+=sizeof(uint32_t);
            	memcpy(claveBloqueada+tamanio,claveStore,string_length(claveStore));
            	tamanio+=string_length(claveStore);

            	sendRemasterizado(socketServerCoordinador,OPERACION_STORE,tamanio,claveBloqueada);
            	log_info(logger,"Envio clave de la operacion STORE al coordinador");
            	free(claveBloqueada);

            	int respuesta = recvDeNotificacion(socketServerCoordinador);
            	log_info(logger,"Recibo respuesta por parte del COORDINADOR de la operacion STORE");

            	sendDeNotificacion(socketServerPlanificador, respuesta);
            	log_info(logger,"Envio respuesta de la operacion STORE a PLANIFICADOR");

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

