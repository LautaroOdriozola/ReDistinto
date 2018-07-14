#include "algoritmosDistribucion.h"


//Algoritmo EL
infoAlgoritmoDistribucion* getInstanciaEquitativeLoad(){
	pthread_mutex_lock(&instConectadas);
	//Saco el primer elemento de la lista y lo guardo.
	infoAlgoritmoDistribucion * info = list_remove(instanciasConectadas,0);

	//Guardo el elemento al final de la lista
	list_add(instanciasConectadas,info);
	pthread_mutex_unlock(&instConectadas);

	return info;
}

//Algoritmo LSU
infoAlgoritmoDistribucion* getInstanciaLeastSpaceUsed(){

	int cantidadDeInstancias = list_size(instanciasConectadas);
	int i;

	for(i=0; i<cantidadDeInstancias; i++){

		//Consigo la info de la posicion
		infoAlgoritmoDistribucion* info = (infoAlgoritmoDistribucion*) list_get(instanciasConectadas,i);

		log_trace(logger, "PREGUNTANDO A INSTANCIA CON SOCKET = %d CUANTAS ENTRADAS LIBRES TIENE");
		sendDeNotificacion(info->socketInstancia, OPERACION_BLOQUES_LIBRES);

		info->cantidadEntradasLibres = recvDeNotificacion(info->socketInstancia);
		log_trace(logger, "CANTIDAD DE ENTRADAS LIBRES %d PARA INSTANCIA CON SOCKET = %d",
				info->cantidadEntradasLibres, info->socketInstancia);
	}

	infoAlgoritmoDistribucion * data = encontrarInstanciaConMayorEspacioLibre();

	return data;
}



bool existeEspacioLibreIgual(infoAlgoritmoDistribucion * instancia1, infoAlgoritmoDistribucion * instancia2){
	return instancia1->cantidadEntradasLibres == instancia2->cantidadEntradasLibres;
}


infoAlgoritmoDistribucion * encontrarInstanciaConMayorEspacioLibre(){

	ordenarListaDeInstanciasDeMayorToMenor();
	int cantidadDeInstancias = list_size(instanciasConectadas);
	infoAlgoritmoDistribucion * instancia1 = (infoAlgoritmoDistribucion *) list_get(instanciasConectadas, 0);

	//Este if es para poder comparar mas de UNA INSTANCIA
	if(cantidadDeInstancias > 1){
		infoAlgoritmoDistribucion * instancia2 = (infoAlgoritmoDistribucion *) list_get(instanciasConectadas, 1);

		//Si las primeras dos instancias son igales defino a elegir por EQUITATIVE LOAD
		if(existeEspacioLibreIgual(instancia1,instancia2)){
			infoAlgoritmoDistribucion * nuevaInstancia = getInstanciaEquitativeLoad();
			return nuevaInstancia;
		} else{
		//Si las primeras dos instancias son distintas elijo la que tiene mayor espacio disponible
			return instancia1;
		}
	// Si existe solo una instancia devuelvo esa
	} else{
		return instancia1;
	}
}


void ordenarListaDeInstanciasDeMayorToMenor(){

	bool compararEntradasLibres(void * info1, void * info2){
		infoAlgoritmoDistribucion* instancia1 = (infoAlgoritmoDistribucion*) info1;
		infoAlgoritmoDistribucion* instancia2 = (infoAlgoritmoDistribucion*) info2;

		return instancia1->cantidadEntradasLibres > instancia2->cantidadEntradasLibres;
	}

	list_sort(instanciasConectadas, compararEntradasLibres);

}
