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
	CANTIDAD_ENTRADAS = recibirUint(socketServerCoordinador);
	log_info(logger,"LA CANTIDAD DE ENTRADAS RECIBIDAS ES: %d.", CANTIDAD_ENTRADAS);
	TAMANIO_ENTRADA = recibirUint(socketServerCoordinador);
	log_info(logger,"EL TAMAÑO DE LAS ENTRADAS ES: %d.", TAMANIO_ENTRADA);
	}

	puts("*************************************************");

	log_info(logger,"CREANDO ESTRUCTURAS ADMINISTRATIVAS!");

	int tamanioTabla = CANTIDAD_ENTRADAS*sizeof(infoTablaDeEntradas);

	log_info(logger,"CREANDO TABLA DE ENTRADAS.");

	tablaDeEntradas = malloc(tamanioTabla);	// Creo malloc para tablaEntradas

	log_info(logger,"TAMAÑO TABLA DE ENTRADAS: %d.", tamanioTabla);

	bitArrayTablaDeEntradas = crearBitArray(CANTIDAD_ENTRADAS);	// Inicializo mi bitArray tabla de entradas

	log_info(logger, "CREANDO BITARRAY DE TABLA DE ENTRADAS.");

	log_info(logger,"BitArray de TABLA DE ENTRADAS creado con EXITO");

	int tamanioStorage = CANTIDAD_ENTRADAS*TAMANIO_ENTRADA;

	log_info(logger,"RESERVANDO MEMORIA PARA MI STORAGE.");

	storage = malloc(tamanioStorage);							// Creo malloc para nuestro Storage

	log_info(logger,"TAMAÑO STORAGE: %d", tamanioStorage);

	bitArrayStorage = crearBitArray(CANTIDAD_ENTRADAS);

	log_info(logger,"CREANDO BITARRAY DE MI STORAGE.");

	log_info(logger, "BitArray de STORAGE creado con EXITO");

	puts("*************************************************");

	log_info("ESPERANDO PARA HACER ALGUNA OPERACION! :D");
}

//Creo bit array. Escribo var global = bitArrayTablaDeEntradas
t_bitarray * crearBitArray(uint32_t cantBloques){

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

	//log_info(logger,"BIT ARRAY CREADO CON EXITO");
	return bitarray;
}

void escribirEnMemoria(infoTablaDeEntradas * datosTablaDeEntrada, char* valor){

	int posicionLibreTablaDeEntradas = devolverPosicionLibreTablaDeEntradas();
	int posicionLibreStorage = datosTablaDeEntrada->nroEntrada;

	int posicionesStorageAOcupar = calcularCantidadDeEntradasAOcupar(valor);
	int i;
	int inicioValor = 0;
	char* valorAux = string_new();


	log_info(logger, "Escribo en TABLA DE ENTRADAS.");
	log_info(logger, "Clave = %s \t Nro de Entrada = %d  \t  Tamaño valor = %d", datosTablaDeEntrada->clave, datosTablaDeEntrada->nroEntrada, datosTablaDeEntrada->tamanioValor);


	//Escribo en mi tabla de entradas
	memcpy(tablaDeEntradas + posicionLibreTablaDeEntradas * sizeof(infoTablaDeEntradas), datosTablaDeEntrada, sizeof(infoTablaDeEntradas));

	// Seteo en el bitArray de T.D.E para decir que esa posicion esta escrita con info.
	bitarray_set_bit(bitArrayTablaDeEntradas, posicionLibreTablaDeEntradas);


	//Bucle para escribir las entradas necesarias.

	for(i=0; i < posicionesStorageAOcupar; i++){
		valorAux = string_substring(valor,inicioValor,TAMANIO_ENTRADA);

		log_info(logger,"Escribo en STORAGE.");
		log_info(logger, "Valor o porcion de valor = %s", valorAux);

		//Escribo en el storage el valor
		memcpy(storage + posicionLibreStorage * string_length(valorAux) , valorAux, string_length(valorAux));

		// Seteo la posicion actual.
		bitarray_set_bit(bitArrayStorage, posicionLibreStorage);

		inicioValor += TAMANIO_ENTRADA;
		posicionLibreStorage += TAMANIO_ENTRADA;

	}


}


int devolverPosicionLibreTablaDeEntradas(){
  int largoBitArray = CANTIDAD_ENTRADAS;
  int posicion = 0;

  while(posicion < largoBitArray){

	  if(!bitarray_test_bit(bitArrayTablaDeEntradas,posicion)){
		  break;
	  } else {
		  posicion++;
	  }

  }// Cierro while

  if(posicion == largoBitArray){
	  // que pasa si me quedo sin espacio para almacenar claves?
	  return -1;
  } else {
	  return posicion;
  }

}

int devolverPosicionLibreStorage(){
	int largoBitArray = CANTIDAD_ENTRADAS;
	int posicion = 0;

	while(posicion < largoBitArray){

		if(!bitarray_test_bit(bitArrayStorage,posicion)){
			break;
		} else {
			posicion++;
		}

	} // Cierro while

	if(posicion == largoBitArray){
	  /*
	   *
	   * COMENZAR A USAR EL ALGORITMO DE REEMPLAZO
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

	//@ PARA TESTEAR
	//string_append(&clave,"futbol");
	//string_append(&valor,"12341234123412341");


	clave = recibirString(socketServerCoordinador);
	valor = recibirString(socketServerCoordinador);


	int cantidadDeEntradas = calcularCantidadDeEntradasAOcupar(valor);

	log_info(logger, "Cantidad de entradas a escribir por %s = %d \n",valor,cantidadDeEntradas);

	int lugarLibreStorage = devolverPosicionLibreStorage();
	infoTablaDeEntradas * infoParaAlmacenar;

	if(cantidadDeEntradas > 1){
		infoParaAlmacenar = crearStrParaAlmacenar(clave, string_length(valor), lugarLibreStorage,VARIAS_ENTRADAS);
	} else{
		infoParaAlmacenar = crearStrParaAlmacenar(clave, string_length(valor), lugarLibreStorage,UNICA_ENTRADA);
	}

	escribirEnMemoria(infoParaAlmacenar, valor);
	free(infoParaAlmacenar);

	/*
	 *
	 * FALTA CONFIRMARLE AL COORDINADOR QUE TUVE EXITO!
	 *
	 *
	 */

	//sendDeNotificacion(socket, OPERACION_EXITO);

}

infoTablaDeEntradas * crearStrParaAlmacenar(char* clave, int largoValor, int posicionEntrada, bool variasEntradas){

	infoTablaDeEntradas * info = (infoTablaDeEntradas*)malloc(sizeof(infoTablaDeEntradas));

	info->clave = string_new();
	string_append(&info->clave,clave);
	info->nroEntrada = posicionEntrada;
	info->tamanioValor = largoValor;
	info->variasEntradas = variasEntradas;

	return info;
}

//Paso como parametro una clave y me devuelve la estructura de esa clave
infoTablaDeEntradas * getInfoTabla(char* claveABuscar){
	  int largoBitArray = CANTIDAD_ENTRADAS;

	  int posicion;
	  infoTablaDeEntradas * datos;

	  // Recorro todo el bitArray
	  for(posicion = 0; posicion < largoBitArray; posicion++){

		  // Si la posicion esta escrita, saco la info de esa posicion.
		  if(bitarray_test_bit(bitArrayTablaDeEntradas,posicion)){

			  // Encuentro la estructura de la posicion escrita.
			  datos = (infoTablaDeEntradas*) (tablaDeEntradas + posicion * sizeof(infoTablaDeEntradas));

			  //Comparo las claves para saber si necesito esta estructura o no.
			  if(strcmp(datos->clave,claveABuscar)==0){		// Comparo strings. Si da 0 es porque son iguales
				  break;
			  }

		  } else {
			  log_info(logger, "No encontre la clave en esa posicion. SIGO BUSCANDO!");
		  }
	  }

	//Cargo una lista vacia y la devuelvo
	if(posicion > largoBitArray){
		log_info(logger,"No existe la clave buscada.");
		datos->clave = "";
		datos->nroEntrada = 0;
		datos->tamanioValor = 0;
		datos->variasEntradas = false;
		return datos;
	} else {
		return datos;
	}
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
