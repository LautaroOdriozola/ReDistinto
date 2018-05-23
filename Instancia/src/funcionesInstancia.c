#include "funcionesInstancia.h"

void cargarConfigInstancia(t_config* configuracion){

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


	if (!config_has_property(configuracion, "ALGORITMO_REEMPLAZO")) {
		printf("No se encuentra el parametro ALGORITMO_REEMPLAZO en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro ALGORITMO_REEMPLAZO en el archivo de configuracion");
		config_destroy(configuracion);
		//liberarMemoria();
		exit(-1);
	}

	ALGORITMO_REEMPLAZO = string_new();
	string_append(&ALGORITMO_REEMPLAZO, config_get_string_value(configuracion, "ALGORITMO_REEMPLAZO"));
	printf("ALGORITMO_REEMPLAZO=%s\n", ALGORITMO_REEMPLAZO);

	if (!config_has_property(configuracion, "PATH_MONTAJE")) {
		printf("No se encuentra el parametro PATH_MONTAJE en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro PATH_MONTAJE en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	PATH_MONTAJE = string_new();
	string_append(&PATH_MONTAJE, config_get_string_value(configuracion, "PATH_MONTAJE"));
	printf("PATH_MONTAJE=%s\n", PATH_MONTAJE);

	if (!config_has_property(configuracion, "NOMBRE_INSTANCIA")) {
		printf("No se encuentra el parametro NOMBRE_INSTANCIA en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro NOMBRE_INSTANCIA en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	NOMBRE_INSTANCIA = string_new();
	string_append(&NOMBRE_INSTANCIA, config_get_string_value(configuracion, "NOMBRE_INSTANCIA"));
	printf("NOMBRE_INSTANCIA=%s\n", NOMBRE_INSTANCIA);

	if (!config_has_property(configuracion, "INTERVALO_DUMP")) {
		printf("No se encuentra el parametro INTERVALO_DUMP en el archivo de configuracion\n");
		log_error(logger,"No se encuentra el parametro INTERVALO_DUMP en el archivo de configuracion");
		//liberarMemoria();
		exit(-1);
	}

	INTERVALO_DUMP = config_get_int_value(configuracion,"INTERVALO_DUMP");
	printf("INTERVALO_DUMP=%d\n", INTERVALO_DUMP);

	config_destroy(configuracion);

}

void iniciarEstructurasAdministrativasInstancia(){
	int notificacion = recvDeNotificacion(socketServerCoordinador);	// Recibo que operacion realizar
	if(notificacion == DATOS_ADMINISTRATIVOS){	// Deberia ser la operacion DATOS_ADMIN para generar estructuras administrativas
	CANTIDAD_ENTRADAS = recibirInt(socketServerCoordinador);
	printf("LA CANTIDAD DE ENTRADAS RECIBIDAS ES: %d \n", CANTIDAD_ENTRADAS);
	TAMANIO_ENTRADA = recibirInt(socketServerCoordinador);
	printf("EL TAMAÑO DE LAS ENTRADAS ES: %d \n", TAMANIO_ENTRADA);
	}

	puts("*************************************************");
	puts("CREANDO ESTRUCTURAS ADMINISTRATIVAS!");
	tablaDeEntradas = malloc(CANTIDAD_ENTRADAS*(sizeof(infoTablaDeEntradas)));	// Creo malloc para tablaEntradas
	int tamanioTabla = CANTIDAD_ENTRADAS*sizeof(infoTablaDeEntradas);
	printf("TAMAÑO TABLA DE ENTRADAS: %d \n", tamanioTabla);
	bitArrayTablaDeEntradas = crearBitArray(CANTIDAD_ENTRADAS);	// Inicializo mi bitArray tabla de entradas
	printf("CREANDO BITARRAY\n");

	storage = malloc(CANTIDAD_ENTRADAS*TAMANIO_ENTRADA);							// Creo malloc para nuestro Storage
	int tamanioStorage = CANTIDAD_ENTRADAS*TAMANIO_ENTRADA;
	printf("TAMAÑO STORAGE: %d \n", tamanioStorage);
	puts("*************************************************");
}

//Creo bit array. Escribo var global = bitArrayTablaDeEntradas
t_bitarray * crearBitArray(int cantBloques){

	int tamanioBitarray=cantBloques/8;
	if(cantBloques % 8 != 0){
	  tamanioBitarray++;
	 }

	char* bits=malloc(tamanioBitarray);

	t_bitarray * bitarray = bitarray_create_with_mode(bits,tamanioBitarray,MSB_FIRST);

	int cont=0;
	for(; cont < tamanioBitarray*8; cont++){
		bitarray_clean_bit(bitarray, cont);
	}

	log_info(logger,"BIT ARRAY CREADO CON EXITO");
	return bitarray;
}

void escribirEnMemoria(infoTablaDeEntradas * datosTablaDeEntrada, char* valor){
	int posicionLibre = datosTablaDeEntrada->nroEntrada;

	printf("Escribo en TABLA DE ENTRADAS.\n");
	printf("Clave = %s \t\t", datosTablaDeEntrada->clave);
	printf("Nro de Entrada = %d \t", datosTablaDeEntrada->nroEntrada);
	printf("Tamaño valor = %d \n", datosTablaDeEntrada->tamanioValor);

	log_info(logger, "Escribo en TABLA DE ENTRADAS.");
	log_info(logger, "Clave = %s", datosTablaDeEntrada->clave );
	log_info(logger, "Nro de Entrada = %d", datosTablaDeEntrada->nroEntrada );
	log_info(logger, "Tamaño valor = %d", datosTablaDeEntrada->tamanioValor);

	printf("Escribo en STORAGE. \n");
	printf("Valor = %s \n", valor);

	log_info(logger,"Escribo en STORAGE.");
	log_info(logger, "Valor = %s. \n", valor);

	memcpy(tablaDeEntradas + posicionLibre * sizeof(infoTablaDeEntradas), datosTablaDeEntrada, sizeof(infoTablaDeEntradas));

	memcpy(storage + posicionLibre * string_length(valor) , valor, string_length(valor));

	bitarray_set_bit(bitArrayTablaDeEntradas, posicionLibre);

}


int devolverPosicionLibre(){
  int largoBitArray = bitarray_get_max_bit(bitArrayTablaDeEntradas);
  int posicion = 0;

  while(posicion < largoBitArray){

    if(!bitarray_test_bit(bitArrayTablaDeEntradas,posicion)){
      break;
    } else {
      posicion++;
    }

  }

  if(posicion == largoBitArray){
	   /*
	   *Iniciar algoritmo reemplazo

	   *
	   * HACER SWITCH PARA CADA ALGORITMO
	   *
	   *
	   *
	   */

	  return -1;
  } else {
	  return posicion;
  }

}


void manejarOperacionSet(){
	char * clave = string_new();
	char * valor = string_new();

	/*
	clave = "futbol";
	valor = "mess1234TRAEMELACO";
	*/

	clave = recibirString(socketServerCoordinador);
	valor = recibirString(socketServerCoordinador);


	int cantidadDeEntradas = calcularCantidadDeEntradasAOcupar(valor);

	printf("Cantidad de entradas a escribir por %s = %d \n",valor,cantidadDeEntradas);
	log_info(logger, "Cantidad de entradas a escribir por %s = %d \n",valor,cantidadDeEntradas);

	if(cantidadDeEntradas == 1){

		int posicionLibreDondeEscribir = devolverPosicionLibre();
		printf("Escribo en la posicion = %d. \n", posicionLibreDondeEscribir);
		log_info(logger, "Escribo en la posicion = %d.", posicionLibreDondeEscribir);
		infoTablaDeEntradas * infoParaAlmacenar = crearStrParaAlmacenar(clave, string_length(valor), posicionLibreDondeEscribir, UNICA_ENTRADA);
		escribirEnMemoria(infoParaAlmacenar, valor);
		free(infoParaAlmacenar);

	} else{
		int inicio = 0;
		int i;
		char* valorAux = string_new();
		for(i= 0;i<cantidadDeEntradas;i++){
			valorAux = string_substring(valor,inicio,TAMANIO_ENTRADA);
			int posicionLibreDondeEscribir = devolverPosicionLibre();
			printf("Escribo en la posicion = %d. \n", posicionLibreDondeEscribir);
			log_info(logger, "Escribo en la posicion = %d.", posicionLibreDondeEscribir);
			infoTablaDeEntradas * infoParaAlmacenar = crearStrParaAlmacenar(clave, string_length(valorAux), posicionLibreDondeEscribir, VARIAS_ENTRADAS);
			escribirEnMemoria(infoParaAlmacenar, valorAux);
			free(infoParaAlmacenar);
			inicio += TAMANIO_ENTRADA;
		}
	}


}

infoTablaDeEntradas * crearStrParaAlmacenar(char* clave, int largoValor, int posicionEntrada, bool excesoDeEntradas){

	infoTablaDeEntradas * info = (infoTablaDeEntradas*)malloc(sizeof(infoTablaDeEntradas));

	info->clave = string_new();
	string_append(&info->clave,clave);
	info->nroEntrada = posicionEntrada;
	info->tamanioValor = largoValor;
	info->unicaEntrada = excesoDeEntradas;

	return info;
}

int calcularCantidadDeEntradasAOcupar(char* palabra){
	int cantidadDeEntradas = string_length(palabra)/TAMANIO_ENTRADA;

	if(string_length(palabra) % TAMANIO_ENTRADA > 0 ){
		cantidadDeEntradas++;
	}

	return cantidadDeEntradas;
}


void liberarMemoriaInstancia(){
	free(tablaDeEntradas);
	free(storage);
	bitarray_destroy(bitArrayTablaDeEntradas);
	log_destroy(logger);
	exit(-1);

}
