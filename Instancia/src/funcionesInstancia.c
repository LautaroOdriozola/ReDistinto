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

	pthread_mutex_init(&mutexTablaDeEntradas, NULL);
	pthread_mutex_init(&mutexStorage, NULL);
	pthread_mutex_init(&mutexDump, NULL);

	int notificacion = recvDeNotificacion(socketServerCoordinador);	// Recibo que operacion realizar
	if(notificacion == DATOS_ADMINISTRATIVOS){	// Deberia ser la operacion DATOS_ADMIN para generar estructuras administrativas
	CANTIDAD_ENTRADAS = recibirUint(socketServerCoordinador);
	log_info(logger,"LA CANTIDAD DE ENTRADAS RECIBIDAS ES: %d.", CANTIDAD_ENTRADAS);
	TAMANIO_ENTRADA = recibirUint(socketServerCoordinador);
	log_info(logger,"EL TAMAÑO DE LAS ENTRADAS ES: %d.", TAMANIO_ENTRADA);
	}

	puts("*************************************************");
	log_trace(logger,"CREANDO ESTRUCTURAS ADMINISTRATIVAS!");
	log_debug(logger,"CREANDO TABLA DE ENTRADAS.");
	tablaDeEntradas = list_create();
	log_info(logger,"TABLA DE ENTRADAS creada con EXITO");
	int tamanioStorage = CANTIDAD_ENTRADAS*TAMANIO_ENTRADA;
	log_info(logger,"RESERVANDO MEMORIA PARA MI STORAGE.");
	storage = malloc(tamanioStorage);							// Creo malloc para nuestro Storage
	log_info(logger,"TAMAÑO STORAGE: %d", tamanioStorage);
	bitArrayStorage = crearBitArray(CANTIDAD_ENTRADAS);
	log_debug(logger,"CREANDO BITARRAY DE MI STORAGE.");
	log_info(logger, "BitArray de STORAGE creado con EXITO");
	puts("*************************************************");

	log_trace(logger, "ESPERANDO PARA HACER ALGUNA OPERACION! :D");
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

	return bitarray;
}

uint32_t escribirEnMemoria(infoTablaDeEntradas * datosTablaDeEntrada, char* valor){

	uint32_t resultado;

	if(strcmp(datosTablaDeEntrada->clave,"")==0){
			log_warning(logger,"No escribo nada ya que no hay informacion");
			resultado = ERROR_DE_INSTANCIA;
	} else {

	int posicionLibreStorage = datosTablaDeEntrada->nroEntrada;
	int posicionesStorageAOcupar = calcularCantidadDeEntradasAOcupar(valor);
	int i;
	int inicioValor = 0;

	log_debug(logger, "Escribo en TABLA DE ENTRADAS.");
	log_trace(logger, "Clave = %s \t Nro de Entrada = %d  \t  Tamaño valor = %d", datosTablaDeEntrada->clave, datosTablaDeEntrada->nroEntrada, datosTablaDeEntrada->tamanioValor);

	//Escribo en mi tabla de entradas
	pthread_mutex_lock(&mutexTablaDeEntradas);
	list_add(tablaDeEntradas, datosTablaDeEntrada);
	pthread_mutex_unlock(&mutexTablaDeEntradas);

	infoPosicion *datos;
	//Bucle para escribir las entradas necesarias.
	for(i=0; i < posicionesStorageAOcupar; i++){
		char* valorAux = string_substring(valor,inicioValor,TAMANIO_ENTRADA);

		infoPosicion * datosValor = crearStrValor(valorAux);

		log_debug(logger,"Escribo en STORAGE.");
		log_info(logger, "Valor o porcion de valor = %s", valorAux);

		//Escribo en el storage el valor
		pthread_mutex_lock(&mutexStorage);
		memcpy(storage + posicionLibreStorage * sizeof(infoPosicion) , datosValor, sizeof(infoPosicion));

		//Para ver que carajo guardo
		datos = (infoPosicion*) (storage + posicionLibreStorage * sizeof(infoPosicion));
		log_info(logger, "ESTOY GUARDANDO ESTO: %s", datos->porcionDeValor);

		log_info(logger, "POSICION LIBRE STORAGE A SETEAR = %d", posicionLibreStorage);
		// Seteo la posicion actual.
		bitarray_set_bit(bitArrayStorage, posicionLibreStorage);
		pthread_mutex_unlock(&mutexStorage);

		inicioValor += TAMANIO_ENTRADA;
		posicionLibreStorage++;
		/******************/
		free(valorAux);
		free(datosValor);
		/*****************/

		}

	resultado = OPERACION_EXITO;
	free(valor);

	}

	return resultado;

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

	if(posicion >= largoBitArray){
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

	char* clave = recibirString(socketServerCoordinador);
	log_info(logger, "Recibo clave %s por parte del COORDINADOR", clave);
	char* valor = recibirString(socketServerCoordinador);
	log_info(logger, "Recibo valor %s por parte del COORDINADOR", valor);

	log_debug(logger,"Chequeando que la CLAVE = %s no contenga un VALOR anteriormente", clave);

	pthread_mutex_lock(&mutexDump);
	if(existeClaveEnTabla(clave)){
		actualizarClaveAndValor(clave,valor);
	} else{
		almacenarClaveAndValor(clave,valor);
	}
	pthread_mutex_unlock(&mutexDump);


}

void actualizarClaveAndValor(char* clave, char* valor){

	infoTablaDeEntradas * info = getInfoTabla(clave);


	int posicionInicial = info->nroEntrada;
	int posicionesABorrar = calcularCantidadPorNumero(info->tamanioValor);
	int i;

	log_warning(logger,"LIMPIANDO POSICIONES anteriormente escritas por la CLAVE = %s", clave);
	for(i=0;i<posicionesABorrar;i++){

		pthread_mutex_lock(&mutexStorage);
		infoPosicion * datos = (infoPosicion*) (storage + posicionInicial * sizeof(infoPosicion));
	 	free(datos->porcionDeValor);
		//free(datos);

		bitarray_clean_bit(bitArrayStorage, posicionInicial);
		pthread_mutex_unlock(&mutexStorage);
		posicionInicial++;
	}

	info->tamanioValor = string_length(valor);
	//int lugarLibreStorage = devolverPosicionLibreStorage();
	//info->nroEntrada = lugarLibreStorage;
	int posicionLibreStorage = info->nroEntrada;
	int posicionesStorageAOcupar = calcularCantidadDeEntradasAOcupar(valor);
	int j;
	int inicioValor = 0;

	log_warning(logger, "ESCRIBIENDO NUEVO VALOR PARA LA CLAVE = %s", clave);
	infoPosicion *datos;
	//Bucle para escribir las entradas necesarias.
	for(j=0; j < posicionesStorageAOcupar; j++){
		char* valorAux = string_substring(valor,inicioValor,TAMANIO_ENTRADA);
		//valorAux =

		infoPosicion * datosValor = crearStrValor(valorAux);

		log_debug(logger,"Escribo en STORAGE.");
		log_info(logger, "Valor o porcion de valor = %s", valorAux);

		//Escribo en el storage el valor
		pthread_mutex_lock(&mutexStorage);
		memcpy(storage + posicionLibreStorage * sizeof(infoPosicion) , datosValor, sizeof(infoPosicion));

		//Para ver que carajo guardo
		datos = (infoPosicion*) (storage + posicionLibreStorage * sizeof(infoPosicion));
		log_info(logger, "ESTOY GUARDANDO ESTO: %s", datos->porcionDeValor);


		log_info(logger, "POSICION LIBRE STORAGE A SETEAR = %d", posicionLibreStorage);
		// Seteo la posicion actual.
		bitarray_set_bit(bitArrayStorage, posicionLibreStorage);
		pthread_mutex_unlock(&mutexStorage);

		inicioValor += TAMANIO_ENTRADA;
		posicionLibreStorage++;
		/******************/
		free(valorAux);
		free(datosValor);
		/*****************/

	}

	if(posicionesStorageAOcupar > 1){
		info->variasEntradas = VARIAS_ENTRADAS;
	} else{
		info->variasEntradas = UNICA_ENTRADA;
	}

	free(valor);
	free(clave);

	//Devuelvo si pude o no escribir en memoria.
	sendDeNotificacion(socketServerCoordinador, OPERACION_EXITO);
}

// CREO ESTRUCTURAS DE TABLA DE ENTRADAS Y PARA ALMACENAR EL VALOR
void almacenarClaveAndValor(char* clave, char* valor){
	if(clave==NULL){
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

	uint32_t resultado = escribirEnMemoria(infoParaAlmacenar, valor);

	free(clave);

	//Devuelvo si pude o no escribir en memoria.
	sendDeNotificacion(socketServerCoordinador, resultado);
	}
}

bool existeClaveEnTabla(char* clave){
	infoTablaDeEntradas * datos = getInfoTabla(clave);
	return datos != NULL;
}

void manejarOperacionStore(){

	log_trace(logger, "REALIZANDO OPERACION STORE!");

	char * clave = recibirString(socketServerCoordinador);
	log_info(logger, "Recibo clave %s por parte del COORDINADOR", clave);

	persistirClave(clave);

	free(clave);
	//Respuesta al coordinador
	sendDeNotificacion(socketServerCoordinador,OPERACION_EXITO);

}

void manejarOperacionBloquesLibres(){

	log_trace(logger, "AVISANDO A COORDINADOR CUANTAS ENTRADAS LIBRES TENGO");

	int cantidadDeEntradasLibres = calcularEntradasLibres();

	sendDeNotificacion(socketServerCoordinador, cantidadDeEntradasLibres);
}

int calcularEntradasLibres(){

	int resultado = 0;
	int i;
	int cantidadDePosiciones = CANTIDAD_ENTRADAS;

	for(i=0;i<cantidadDePosiciones;i++){

		// Si la posicion del bitarray NO esta SETEADA es porque esta libre.
		if(!bitarray_test_bit(bitArrayStorage,i)){
			resultado++;
		}
	}

	return resultado;
}

void* realizarDump(){

	int corte = 1;
	while(corte){
		sleep(INTERVALO_DUMP);
		funcionDump();
	}

	return 0;
}



void funcionDump(){
	  int largo = list_size(tablaDeEntradas);

	  if(largo > 0){
		  log_warning(logger, "COMIENZO A REALIZAR FUNCION DUMP!");

		  void persistirTodasLasClaves(infoTablaDeEntradas * data){
			  persistirClave(data->clave);
		  }

		  pthread_mutex_lock(&mutexDump);
		  list_iterate(tablaDeEntradas ,(void*) persistirTodasLasClaves);
		  pthread_mutex_unlock(&mutexDump);

		  /*
		  pthread_mutex_lock(&mutexTablaDeEntradas);
		  // Recorro todo el bitArray
		  for(posicion = 0; posicion < largo; posicion++){

			  infoTablaDeEntradas * datos = (infoTablaDeEntradas *) list_get(tablaDeEntradas, posicion);

			  log_error(logger,"CLAVE = %s\t NRO DE ENTRADA = %d\t TAMANIO VALOR = %d", datos->clave, datos->nroEntrada, datos->tamanioValor);

			  persistirClave(datos->clave);

		  }
		  pthread_mutex_unlock(&mutexTablaDeEntradas);
		   */
		  log_warning(logger, "TERMINÉ DE REALIZAR EL DUMP");
	  }


}

void persistirClave(char* clave){

	//Busco estructura de la clave en tabla de entradas.
	infoTablaDeEntradas * info = getInfoTabla(clave);

	if(info == NULL){
		log_error(logger, "NO SE ENCONTRÓ LA CLAVE EN LA TABLA DE ENTRADAS.");
		log_warning(logger, "NO SE PUDO PERSISTIR LA CLAVE = %s O YA FUE PERSISTIDA POR EL DUMP", clave);
	} else{
		//Replico estructura para encontrar la clave.
		char* valorAPersistir = getValor(info);

		int posicionInicial = info->nroEntrada;
		int posicionesABorrar = calcularCantidadPorNumero(info->tamanioValor);
		int i;

		log_warning(logger,"LIMPIANDO POSICIONES anteriormente escritas por la CLAVE = %s", clave);
		for(i=0;i<posicionesABorrar;i++){

			pthread_mutex_lock(&mutexStorage);
			infoPosicion * datos = (infoPosicion*) (storage + posicionInicial * sizeof(infoPosicion));
		 	free(datos->porcionDeValor);
			//free(datos);

			bitarray_clean_bit(bitArrayStorage, posicionInicial);
			pthread_mutex_unlock(&mutexStorage);
			posicionInicial++;
		}

		// Creo un string para darle nombre al archivo y hacerlo .txt
		char * nombreArchivo = string_duplicate(clave);
		string_append(&nombreArchivo, ".txt");

		// Hago copia del path de montaje para guardar ahi el archivo
		char* pathRelativo = string_duplicate(PATH_MONTAJE);
		log_info(logger, "Guardando archivo %s en el PATH ABSOLUTO = %s", nombreArchivo, pathRelativo);
		string_append(&pathRelativo, nombreArchivo);

		log_info(logger, "Persistiendo...\tCLAVE = %s\tVALOR = %s", clave, valorAPersistir);

		FILE* fichero = fopen(pathRelativo, "w");

		if(fichero == NULL){
			log_error(logger, "No se pudo crear archivo de nombre %s", nombreArchivo);
		}

		//Escribo el valor en el fichero creado.
		txt_write_in_file(fichero, valorAPersistir);
		//Cierro fichero
		txt_close_file(fichero);

		free(nombreArchivo);
		free(pathRelativo);
		free(valorAPersistir);

		log_warning(logger, "BORRANDO EN TABLA DE CLAVES LA CLAVE = %s", clave);
		borrarClaveDeTabla(info);
	}

	//free(clave);

}


void chequearClave(char * clave){

	infoTablaDeEntradas * info = getInfoTabla(clave);

	if(info == NULL){
		log_debug(logger, "NO EXISTE LA CLAVE = %s EN EL STORAGE.", clave);
	} else{
		int posicionInicial = info->nroEntrada;
		int largoCadena = info->tamanioValor;
		int posicionesABorrar = calcularCantidadPorNumero(largoCadena);
		int i;

		log_debug(logger,"LIMPIANDO POSICIONES anteriormente escritas por la CLAVE = %s", clave);
		for(i=0;i<posicionesABorrar;i++){
			pthread_mutex_lock(&mutexStorage);
			//infoPosicion * datos = (infoPosicion*) (storage + posicionInicial * sizeof(infoPosicion));
			//free(datos->porcionDeValor);
			//free(datos);

			bitarray_clean_bit(bitArrayStorage, posicionInicial);
			pthread_mutex_unlock(&mutexStorage);
			posicionInicial++;
		}

		borrarClaveDeTabla(info);
	}

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

	bool contieneString(void * info){
		infoTablaDeEntradas * data = (infoTablaDeEntradas*) info;
		return strcmp(data->clave, claveABuscar)==0;
	}

	return (infoTablaDeEntradas*) list_find(tablaDeEntradas, (void*) contieneString);

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

void borrarClaveDeTabla(infoTablaDeEntradas* info){

	int posicion = encontrarPosicion(info);

	void liberarInfo(infoTablaDeEntradas * data){
		free(data->clave);
		free(data);
	}

	pthread_mutex_lock(&mutexTablaDeEntradas);
	list_remove_and_destroy_element(tablaDeEntradas, posicion, (void*) liberarInfo);
	pthread_mutex_unlock(&mutexTablaDeEntradas);

}

int encontrarPosicion(infoTablaDeEntradas * info){
	int largo = list_size(tablaDeEntradas);
	int pos;

	for(pos=0; pos<largo;pos++){
		infoTablaDeEntradas* data = (infoTablaDeEntradas*)list_get(tablaDeEntradas,pos);

		if(strcmp(data->clave,info->clave)==0){
			break;
		}
	}

	return pos;
}

void liberarInfoTabla(infoTablaDeEntradas * info){
	free(info->clave);
	free(info);
}

void liberarStorage(){

	int i;
	int cantidadDePosiciones = CANTIDAD_ENTRADAS;

	for(i=0;i<cantidadDePosiciones;i++){

		if(bitarray_test_bit(bitArrayStorage,i)){

			infoPosicion * datos = (infoPosicion*) (storage + i * sizeof(infoPosicion));
			free(datos->porcionDeValor);
			//free(datos);

			bitarray_clean_bit(bitArrayStorage, i);
		}
	}

	free(storage);

}

void liberarMemoriaInstancia(){
	log_error(logger, "Muriendo LENTAMENTE..");

	free(COORDINADOR_IP);
	free(ALGORITMO_REEMPLAZO);
	free(PATH_MONTAJE);
	free(NOMBRE_INSTANCIA);

	log_error(logger, "DESTRUYENDO HILOS Y SEMAFOROS");
	pthread_mutex_destroy(&mutexTablaDeEntradas);
	pthread_mutex_destroy(&mutexStorage);
	pthread_mutex_destroy(&mutexDump);

	pthread_cancel(hiloDump);

	list_destroy_and_destroy_elements(tablaDeEntradas, (void*) liberarInfoTabla);
	liberarStorage();
	bitarray_destroy(bitArrayStorage);
	close(socketServerCoordinador);
	log_error(logger,"INSTANCIA DICE ADIOS");
	log_destroy(logger);
	exit(-1);
}
