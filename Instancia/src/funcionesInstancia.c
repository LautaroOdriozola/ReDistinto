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

	log_info(logger, "ESPERANDO PARA HACER ALGUNA OPERACION! :D");
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

	free(bits);
	return bitarray;
}

void escribirEnMemoria(infoTablaDeEntradas * datosTablaDeEntrada, char* valor){

	if(strcmp(datosTablaDeEntrada->clave,"")==0){
			log_warning(logger,"No escribo nada ya que no hay informacion");

	} else {

	int posicionLibreTablaDeEntradas = devolverPosicionLibreTablaDeEntradas();
	int posicionLibreStorage = datosTablaDeEntrada->nroEntrada;

	int posicionesStorageAOcupar = calcularCantidadDeEntradasAOcupar(valor);
	int i;
	int inicioValor = 0;
	char* valorAux = string_new();


	log_debug(logger, "Escribo en TABLA DE ENTRADAS.");
	log_trace(logger, "Clave = %s \t Nro de Entrada = %d  \t  Tamaño valor = %d", datosTablaDeEntrada->clave, datosTablaDeEntrada->nroEntrada, datosTablaDeEntrada->tamanioValor);


	//Escribo en mi tabla de entradas
	memcpy(tablaDeEntradas + posicionLibreTablaDeEntradas * sizeof(infoTablaDeEntradas), datosTablaDeEntrada, sizeof(infoTablaDeEntradas));

	// Seteo en el bitArray de T.D.E para decir que esa posicion esta escrita con info.
	bitarray_set_bit(bitArrayTablaDeEntradas, posicionLibreTablaDeEntradas);

	infoPosicion *datos;
	//Bucle para escribir las entradas necesarias.

	for(i=0; i < posicionesStorageAOcupar; i++){
		valorAux = string_substring(valor,inicioValor,TAMANIO_ENTRADA);

		infoPosicion * datosValor = crearStrValor(valorAux);

		log_debug(logger,"Escribo en STORAGE.");
		log_info(logger, "Valor o porcion de valor = %s", valorAux);

		//Escribo en el storage el valor
		memcpy(storage + posicionLibreStorage * sizeof(infoPosicion) , datosValor, sizeof(infoPosicion));

		//Para ver que carajo guardo
		datos = (infoPosicion*) (storage + posicionLibreStorage * sizeof(infoPosicion));
		log_info(logger, "ESTOY GUARDANDO ESTO: %s", datos->porcionDeValor);


		log_info(logger, "POSICION LIBRE STORAGE A SETEAR = %d", posicionLibreStorage);
		// Seteo la posicion actual.
		bitarray_set_bit(bitArrayStorage, posicionLibreStorage);



		inicioValor += TAMANIO_ENTRADA;
		posicionLibreStorage++;

		}
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

	log_trace(logger, "REALIZANDO OPERACION SET!");

	/*@ PARA TESTEAR

	char * clave = string_new();
	char * valor = string_new();

	string_append(&clave,"futbol");
	string_append(&valor,"12341234123412341");*/


	char* clave = recibirString(socketServerCoordinador);
	log_info(logger, "Recibo clave %s por parte del COORDINADOR", clave);
	char* valor = recibirString(socketServerCoordinador);
	log_info(logger, "Recibo valor %s por parte del COORDINADOR", valor);

	if(strcmp(clave,"")==0){
		log_warning(logger,"No escribo nada ya que no hay informacion");
		//notificacion = FRACASO; 		// Esto sirve para replicarle al coordinador que no pude hacer el set de una clave vacia
	} else {

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

	sendDeNotificacion(socketServerCoordinador, OPERACION_EXITO);
	}

}

void manejarOperacionStore(){

	log_info(logger, "REALIZANDO OPERACION STORE!");

	char * clave = recibirString(socketServerCoordinador);
	log_info(logger, "Recibo clave %s por parte del COORDINADOR", clave);

	persistirClave(clave);

	//Respuesta al coordinador
	//sendDeNotificacion(socketServerCoordinador,OPERACION_EXITO);

}

void realizarDump(){
	  int largoBitArray = CANTIDAD_ENTRADAS;

	  int posicion;
	  infoTablaDeEntradas * datos;

	  log_trace(logger,"REALIZANDO FUNCION DUMP!");

	  // Recorro todo el bitArray
	  for(posicion = 0; posicion < largoBitArray; posicion++){

		  // Si la posicion esta escrita, saco la info de esa posicion.
		  if(bitarray_test_bit(bitArrayTablaDeEntradas,posicion)){

			  // Encuentro la estructura de la posicion escrita.
			  datos = (infoTablaDeEntradas*) (tablaDeEntradas + posicion * sizeof(infoTablaDeEntradas));

			  log_debug(logger,"CLAVE = %s\t NRO DE ENTRADA = %d\t TAMANIO VALOR = %d", datos->clave, datos->nroEntrada, datos->tamanioValor);

			  persistirClave(datos->clave);
		  }
	  }
}

void persistirClave(char* clave){

	//Busco estructura de la clave en tabla de entradas.
	infoTablaDeEntradas * info = getInfoTabla(clave);

	//Replico estructura para encontrar la clave.
	char* valorAPersistir = getValor(info);


	// Creo un string para darle nombre al archivo y hacerlo .txt
	char * nombreArchivo = string_duplicate(clave);
	string_append(&nombreArchivo, ".txt");

	// Hago copia del path de montaje para guardar ahi el archivo
	char* pathRelativo = string_duplicate(PATH_MONTAJE);
	log_info(logger, "Guardando archivo %s en el PATH ABSOLUTO = %s", nombreArchivo, pathRelativo);
	string_append(&pathRelativo, nombreArchivo);

	log_info(logger, "Persistiendo...\tCLAVE = %s\tVALOR = %s!", clave, valorAPersistir);

	FILE* fichero = txt_open_for_append(pathRelativo);

	if(fichero == NULL){
		log_error(logger, "No se pudo crear archivo de nombre %s", nombreArchivo);
	}

	//Escribo el valor en el fichero creado.
	txt_write_in_file(fichero, valorAPersistir);
	//Cierro fichero
	txt_close_file(fichero);

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

//Paso info de TABLA DE ENTRADAS y me devuelve el valor almacenado en STORAGE.
char * getValor(infoTablaDeEntradas * info){

	int i;
	int posicion = info->nroEntrada;
	int largoCadena = info->tamanioValor;
	int nroEntradas = calcularCantidadPorNumero(largoCadena);

	infoPosicion * datos;
	char * valorAux = string_new();

	log_info(logger, "Armando el valor...\n");

	for(i=0;i<nroEntradas;i++){

		//TODO: fijarse si es negado o no.
		if(bitarray_test_bit(bitArrayStorage,posicion)){
			//Saco datos de la posicion
			datos = (infoPosicion*) (storage + posicion * sizeof(infoPosicion));

			log_info(logger,"Porcion de valor hallado = %s.  \n", datos->porcionDeValor);

			string_append(&valorAux, datos->porcionDeValor);

			posicion++;

		}
	}


	log_info(logger,"Valor hallado = %s.  \n", valorAux);

	//Catcheo error
	if(string_length(valorAux) == 0){
		return "";
	} else{
		return valorAux;
	}

}

// Calculo a traves de un string
int calcularCantidadDeEntradasAOcupar(char* palabra){
	int cantidadDeEntradas = string_length(palabra)/TAMANIO_ENTRADA;

	if(string_length(palabra) % TAMANIO_ENTRADA > 0 ){
		cantidadDeEntradas++;
	}

	return cantidadDeEntradas;
}

//Calculo a traves del largo
int calcularCantidadPorNumero(int largoClave){
	int cantidadDeEntradas = largoClave/TAMANIO_ENTRADA;

	if(largoClave % TAMANIO_ENTRADA > 0 ){
		cantidadDeEntradas++;
	}

	return cantidadDeEntradas;
}

// Cargo valor o porcion de valor en una estructura para meterla en el storage
infoPosicion * crearStrValor(char* palabra){

	infoPosicion * datos = (infoPosicion*)malloc(sizeof(infoPosicion));

	datos->porcionDeValor = string_new();
	string_append(&datos->porcionDeValor, palabra);

	return datos;
}

void liberarMemoriaInstancia(){
	free(tablaDeEntradas);
	free(storage);
	bitarray_destroy(bitArrayTablaDeEntradas);
	bitarray_destroy(bitArrayStorage);
	log_destroy(logger);
	exit(-1);
}
