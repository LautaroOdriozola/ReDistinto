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

void contarLineas(FILE * archivo){
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    printf("*************************************** \n");
    log_debug(logger, "IMPRIMIENDO SCRIPT A EJECUTAR");
    printf("*************************************** \n");

    while ((read = getline(&line, &len, archivo)) != -1) {
        t_esi_operacion * parsed = malloc(sizeof(t_esi_operacion));
        *parsed = parse(line);

        list_add(listaDeInstrucciones,parsed);

        if(parsed->valido){
            switch(parsed->keyword){
                case GET:
                    printf("GET\tclave: <%s>\n", parsed->argumentos.GET.clave);
                    break;
                case SET:
                    printf("SET\tclave: <%s>\tvalor: <%s>\n", parsed->argumentos.SET.clave, parsed->argumentos.SET.valor);
                    break;
                case STORE:
                    printf("STORE\tclave: <%s>\n", parsed->argumentos.STORE.clave);
                    break;
                default:
                    fprintf(stderr, "No pude interpretar <%s>\n", line);
                	liberarMemoriaESI();
            }

            //destruir_operacion(parsed);

        } else {
            fprintf(stderr, "La linea <%s> no es valida\n", line);
        	liberarMemoriaESI();
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

void manejarOperacionDeParseo(){

	if(list_size(listaDeInstrucciones) > 0){

		t_esi_operacion * parsed = (t_esi_operacion *) list_get(listaDeInstrucciones,INICIO_DE_INSTRUCCION);

	    if(parsed->valido && parsed != NULL){

	    	log_warning(logger,"PARSEANDO LINEA...");
	    	usleep(100);
	    	//sleep(1);

	        switch(parsed->keyword){

	        	case GET:
	            	log_trace(logger, "Realizo operacion GET");
	            	log_debug(logger,"GET\tclave: <%s>\n", parsed->argumentos.GET.clave);

	            	char * claveGet = string_new();
	            	string_append(&claveGet,parsed->argumentos.GET.clave);

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

	            		case OPERACION_EXITO:
	            			log_debug(logger, "COORDINADOR ME DIJO QUE PUDO OPERAR CORRECTAMENTE!");
	            			break;

	            		case ERROR_TAMANIO_CLAVE:
	            			//liberarMemoriaESI();
	            			break;

	            		case BLOQUEAR_ESI:
	            			log_debug(logger, "COORDINADOR ME DIJO QUE ME VOY A BLOQUEAR");

	            			INICIO_DE_INSTRUCCION--;
	            			break;

	            		case 0:
	            			break;

	            		default:
	            			break; //Si no es ningun error, sigo con el flujo normal?

	            	}


	            	sendDeNotificacion(socketServerPlanificador, respuestaGet);
	            	log_info(logger,"Envio respuesta de la operacion GET a PLANIFICADOR");
	                free(claveGet);
	            	break;

	            case SET:
	              	log_trace(logger, "Realizo operacion SET");
	              	log_debug(logger,"SET\tclave: <%s>\tvalor: <%s>\n", parsed->argumentos.SET.clave, parsed->argumentos.SET.valor);

	              	char* claveSet = string_new();
	              	string_append(&claveSet,parsed->argumentos.SET.clave);
	              	char* valor = string_new();
	              	string_append(&valor,parsed->argumentos.SET.valor);

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

	              	sendDeNotificacion(socketServerPlanificador, respuestaSet);
	              	log_info(logger,"Envio respuesta de la operacion SET a PLANIFICADOR");

	              	switch(respuestaSet){

	              		case ERROR_DE_INSTANCIA:
	              			liberarMemoriaESI();
	              			break;

	              		case ERROR_CLAVE_NO_IDENTIFICADA:
	              			log_error(logger,"ERROR DE CLAVE NO IDENTIFICADA");
	              			liberarMemoriaESI();
	              			break;
	              	}

	              	free(claveSet);
	              	free(valor);
	              	break;

	            case STORE:

	            	log_trace(logger, "Realizo operacion STORE");
	            	log_debug(logger,"STORE\tclave: <%s>\n", parsed->argumentos.STORE.clave);
	            	char* claveStore = string_new();
	            	string_append(&claveStore,parsed->argumentos.STORE.clave);

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

	            	switch(respuesta){
	            		case OPERACION_EXITO:
	            			log_debug(logger, "COORDINADOR me dice que pudo realizar STORE correctamente");
	            			sendDeNotificacion(socketServerPlanificador, respuesta);
	            			log_info(logger,"Envio respuesta de la operacion STORE a PLANIFICADOR");
	            			break;

	            		case ABORTAR_ESI:
	            			log_error(logger, "COORDINADOR me dice que ABORTE");
	            			sendDeNotificacion(socketServerPlanificador, respuesta);
	            			log_info(logger,"Envio respuesta de la operacion STORE a PLANIFICADOR");
	            			log_error(logger, "Muriendo lentamente...");
	            			liberarMemoriaESI();
	            			break;

	              		case ERROR_DE_INSTANCIA:
	              			log_error(logger,"ERROR DE INSTANCIA AL HACER EL STORE");
	              			sendDeNotificacion(socketServerPlanificador, respuesta);
	              			liberarMemoriaESI();
	              			break;

	              		case ERROR_CLAVE_NO_IDENTIFICADA:
	              			sendDeNotificacion(socketServerPlanificador, respuesta);
	              			log_error(logger,"ERROR DE CLAVE NO IDENTIFICADA");
	              			liberarMemoriaESI();
	              			break;


	            		default:
	            			break;
	            	}

	            	free(claveStore);
	               	break;

	            default:

	            	log_error(logger, "No pude interpretar");
	            	liberarMemoriaESI();
	        	}

	        //destruir_operacion(parsed);

	    } else {
	        log_error(logger, "FINALIZANDO ESI NRO = %d", ID_ESI);
	        liberarMemoriaESI();
	    }

	    //Para leer la siguiente instruccion guardada
	    INICIO_DE_INSTRUCCION++;
	}

}

void liberarInstrucciones(t_esi_operacion * parsed){
	if(parsed->_raw){
		string_iterate_lines(parsed->_raw, (void*) free);
		free(parsed->_raw);
	}
	free(parsed);
}

void liberarMemoriaESI(){

	list_destroy_and_destroy_elements(listaDeInstrucciones, (void*) liberarInstrucciones);
	close(socketServerPlanificador);
	close(socketServerCoordinador);
	log_destroy(logger);
	exit(-1);

}

