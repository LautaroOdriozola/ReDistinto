#include "algoritmosDePlanificacion.h"

void imprimirColaReady(){

	log_debug(logger, "IMPRIMIENDO LISTA DE READY");

	int largo = list_size(listaListos);
	int i;

	log_debug(logger, "NRO DE ORDEN: \t ESI NRO: \t ESTIMACION:");

	for(i=0;i<largo;i++){
		infoESI * ESI = (infoESI*) list_get(listaListos,i);
		log_debug(logger, "%d \t\t %d \t\t %.2f",i, ESI->ID_ESI, ESI->estimacion);
	}

}

void ordenarColaReady(){
	// ordeno cola de ready segun el algoritmo
	if(strcmp(ALGORITMO_PLANIFICACION,"SJF-SD") == 0 ){
		if(ESI_EJECUTANDO == NULL || ESI_EJECUTANDO->estado == finalizado || ESI_EJECUTANDO->estado == ejecutando){
		} else{
			ordenarPorSjf_Sd();
		}
	} else if( strcmp(ALGORITMO_PLANIFICACION,"SJF-CD") == 0 ){
		ordenarPorSjf_Cd();		/* SIN IMPLEMENTAR TODAVIA */
	} else if( strcmp(ALGORITMO_PLANIFICACION,"HRRN") == 0 ){
		//ordenarPorHRRN();			/* SIN IMPLEMENTAR TODAVIA */
	}
}

void ordenarPorSjf_Sd(){
	//Funcion auxiliar para el sort
	bool ordenarESIs(infoESI* primero,infoESI* segundo){
		if(primero->estimacion == segundo->estimacion){
			return primero->posicionEnLista < segundo->posicionEnLista;
		} else{
			return compararEstimaciones(primero,segundo);
		}
	}
	list_sort(listaListos, (void*) ordenarESIs);
}

void ordenarPorSjf_Cd(){
	//Funcion auxiliar para el sort
	bool ordenarESIs(infoESI* primero,infoESI* segundo){
		if(primero->estimacion == segundo->estimacion){
			return primero->posicionEnLista < segundo->posicionEnLista;
		} else{
			return compararEstimaciones(primero,segundo);
		}
	}
	list_sort(listaListos, (void*) ordenarESIs);
}

//Funcion auxiliar para hacer el sort de la lista
bool compararEstimaciones(infoESI* primero, infoESI* segundo){
	return primero->estimacion < segundo->estimacion;
}


void estimarListos(){
	int largo = list_size(listaListos);
	int i;

	for(i=0; i<largo ; i++){

		infoESI* ESI = (infoESI*) list_get(listaListos, i);

		estimarRafaga(ESI);
	}
}

void estimarRafaga(infoESI * ESI){

	log_warning(logger, "ESTIMANDO ESI %d", ESI->ID_ESI );

	//log_warning(logger, "RAFAGA ANTERIOR = %.2f", ESI->duracionDeRafaga);

	//log_warning(logger, "ESTIMACION ANTERIOR = %.2f", ESI->estimacion);

	float duracionDeRafaga = ESI->duracionDeRafaga; //Duracion de la rafaga anterior

	float estimacionInicial = ESI->estimacion; // Estimacion anterior

	float estimacionNueva = ((ALFA_PLANIFICACION / 100.0)* duracionDeRafaga) + ((1 - (ALFA_PLANIFICACION / 100.0))* estimacionInicial);

	ESI->estimacion = estimacionNueva;

	//log_warning(logger, "ESTIMACION POSTERIOR = %.2f", ESI->estimacion);
}
