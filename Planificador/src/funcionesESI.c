#include "funcionesESI.h"
#include "algoritmosDePlanificacion.h"


void iniciarESI(int socket){

	infoESI * nuevoESI = malloc(sizeof(infoESI));

	nuevoESI->socketESI=socket;
	nuevoESI->duracionDeRafaga=0;
	nuevoESI->estimacion=ESTIMACION_INICIAL + 0.0;
	nuevoESI->posicionEnLista = posicionListaListos;

	posicionListaListos++;

	//Recibo el ID del ESI
	uint32_t id = recibirUint(socket);
	nuevoESI->ID_ESI=id;
	log_debug(logger,"Se ha creado el ESI N° %d",id);

	//Recibo cantidad de instrucciones a ejecutar
	uint32_t cantidadInstrucciones = recibirUint(socket);
	nuevoESI->cantidadDeInstrucciones = cantidadInstrucciones;

	//Agrego nuevo ESI a la cola de ready
	log_trace(logger, "Encolando ESI = %d -> COLA DE READY", nuevoESI->ID_ESI);
	//nuevoESI->estado = listo;
	pthread_mutex_lock(&colaReady);
	list_add(listaListos, (void*) nuevoESI);
	pthread_mutex_unlock(&colaReady);
	sem_post(&esiListos);

}


void ejecutarSentencia(infoESI * str){
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	log_debug(logger, "EJECUTANDO ESI NRO = %d", str->ID_ESI);
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	sendDeNotificacion(str->socketESI,PARSEAR_LINEA);
}

void manejarESI(int socketESI){

	//Inicio estructuras del socket
	iniciarESI(socketESI);

	while(1){

		uint32_t resultadoOperacion = recvDeNotificacion(socketESI);

		switch(resultadoOperacion){

			case OPERACION_EXITO:
				ejecucionDeESIExitosa();
				sem_post(&semPlanificar);
				break;

			case BLOQUEAR_ESI:
				log_trace(logger, "Encolando ESI = %d -> COLA DE BLOQUEADOS", ESI_EJECUTANDO->ID_ESI);
				sem_post(&semPlanificar);
				break;

			case ABORTAR_ESI:
				log_error(logger, "ME AVISAN QUE EL ESI SE ABORTO. ELIMINANDOLO DE MIS ESTRUCTURAS");
				eliminarHiloDeConexion(socketESI);
				sem_post(&semPlanificar);
				break;

			case 0:
				//eliminarHiloDeConexion(socketESI);
				close(socketESI);
				break;

			default:
				//eliminarHiloDeConexion(socketESI);
				close(socketESI);
		}

	}

}

void eliminarHiloDeConexion(int socketESI){

	bool encontrarSocket(infoHilos * info){
		return info->socket == socketESI;
	}

	pthread_mutex_lock(&mutexHilos);
	infoHilos * data = (infoHilos*) list_remove_by_condition(hilosParaConexiones,(void*) encontrarSocket);
	pthread_mutex_unlock(&mutexHilos);

	if(data == NULL){
		log_error(logger, "No se encontro el hilo de conexion que atendia al socket %d", socketESI);
	} else{
		log_warning(logger, "Eliminando hilo con conexion en socket = %d", socketESI);
		log_warning(logger, "Socket cerro la conexion!");

		pthread_cancel(data->hiloAtendedor);
		free(data);
	}
}

void ejecucionDeESIExitosa(){

	int id = ESI_EJECUTANDO->ID_ESI;
	log_debug(logger, "ESI NRO = %d PUDO OPERAR CORRECTAMENTE", id);

	pthread_mutex_lock(&ESI_EXECUTOR);
	ESI_EJECUTANDO->cantidadDeInstrucciones --;
	pthread_mutex_unlock(&ESI_EXECUTOR);

	estimarRafaga(ESI_EJECUTANDO);

	if(ESI_EJECUTANDO->cantidadDeInstrucciones == 0){
		pasarESIAFinalizado(ESI_EJECUTANDO);
		ESI_EJECUTANDO->estado = finalizado;
		//sem_post(&esiListos);
	} else{
		log_trace(logger, "Encolando ESI = %d -> COLA DE READY", ESI_EJECUTANDO->ID_ESI);
		//ESI_EJECUTANDO->estado = listo;
		ESI_EJECUTANDO->posicionEnLista = posicionListaListos;
		sem_post(&esiListos);
	}


}

/*****************************************************
 * LA LISTA DE ESIS BLOQUEADOS TENDRA ESTAS DOS COSAS:
 *
 * - CLAVE POR LA CUAL SE BLOQUEO.
 * - INFORMACION COMPLETA DEL ESI (infoESI*)
 *
 ****************************************************/

/*
void bloquearESIConClave(char* claveCausanteDeBloqueo, int ID_ESI_A_BLOQUEAR){

	int idDelEjecutando = ESI_EJECUTANDO->ID_ESI;

	if(idDelEjecutando == ID_ESI_A_BLOQUEAR){

		log_debug(logger, "BLOQUEANDO ESI = %d", idDelEjecutando);
		infoESIBloqueado * info = (infoESIBloqueado*) malloc(sizeof(infoESIBloqueado));

		info->claveCausante = string_new();
		string_append(&info->claveCausante, claveCausanteDeBloqueo);
		info->ESI = ESI_EJECUTANDO;

		pthread_mutex_lock(&colaBloqueados);
		list_add(listaBloqueados, (void*)info);
		pthread_mutex_unlock(&colaBloqueados);

		if (ESI_EJECUTANDO->estado==listo) {
			//sem_wait(&esiListos);
		}
		ESI_EJECUTANDO->estado =bloqueado;


	} else{
		log_error(logger, "NO SE ENCONTRO EL ESI BUSCADO PARA PODER BLOQUEARLO.");
	}
}*/

void bloquearESI(infoESI* ESI, char * clave){
	log_debug(logger, "SACANDO ESI = %d DE COLA DE LISTOS", ESI->ID_ESI);
	ESI = removerESI(listaListos, ESI->ID_ESI);

	log_debug(logger, "BLOQUEANDO ESI = %d", ESI->ID_ESI);
	infoESIBloqueado * info = (infoESIBloqueado*) malloc(sizeof(infoESIBloqueado));
	info->claveCausante = string_new();
	string_append(&info->claveCausante, clave);
	info->ESI = ESI;
	pthread_mutex_lock(&colaBloqueados);
	list_add(listaBloqueados, info);
	pthread_mutex_unlock(&colaBloqueados);
	ESI->estado =bloqueado;
	//free(clave);
}

void bloquearClave(char* clave,int id){
	log_debug(logger,"BLOQUEANDO CLAVE = %s", clave);
	infoClaveBloqueada * data = (infoClaveBloqueada*) malloc(sizeof(infoClaveBloqueada));
	data->claveBloqueada = string_new();
	string_append(&data->claveBloqueada, clave);
	data->ID_ESI = id;
	pthread_mutex_lock(&clavesBloqueadas);
	list_add(listaClavesBloqueadas, data);
	pthread_mutex_unlock(&clavesBloqueadas);

}

void pasarESIABloqueado(char * clave, int id){
	infoESI * ESI = buscarESIxID(id);

	if(ESI == NULL){
		log_error(logger,"ESI = %d NO EXISTE", id);
	} else{
		if(existeClaveEnLista(clave)){
			bloquearESI(ESI,clave);
		} else{
			bloquearClave(clave,id);
			bloquearESI(ESI,clave);
		}
	}
}


void pasarESIAListo(infoESI * ESI){

	//Agrego nuevo ESI a la cola de ready
	log_trace(logger, "Encolando ESI = %d -> COLA DE READY", ESI->ID_ESI);
	ESI->estado = listo;
	/*pthread_mutex_lock(&colaReady);
	list_add(listaListos, (void*)ESI);
	pthread_mutex_unlock(&colaReady);*/
	//Habilito para poder planificar
	//sem_post(&esiListos);
}

void pasarESIAFinalizado(infoESI * ESI){

	//COMUNICARLE AL COORDINADOR QUE MURIO UN ESI
	//sendDeNotificacion(socketServerCoordinador, ABORTAR_ESI);
	//sendDeNotificacion(socketServerCoordinador, ESI->ID_ESI);

	// DECIRLE AL MISMO ESI QUE MUERA.
	// ESTO LO DEBERIA HACER EL COORDINADOR!!!!
	log_error(logger,"DANDO SEÑAL A ESI NRO = %d QUE FINALICE.",ESI->ID_ESI);
	sendDeNotificacion(ESI->socketESI, ABORTAR_ESI);

	//Eliminar ESI cola en la que se encuentra
	pasarFinalizado(ESI);
}
/*
void eliminarDeColaActual(infoESI * ESI){
	int estado = ESI->estado;

	switch(estado){

		case listo:
			pthread_mutex_lock(&colaReady);
			pasarFinalizado(listaListos, ESI);
			pthread_mutex_unlock(&colaReady);
			break;

		case bloqueado:
			//pthread_mutex_lock(&colaBloqueados);
			//pasarFinalizado(listaBloqueados,ESI);
			//pthread_mutex_unlock(&colaBloqueados);
			break;

		default:
			break;
	}
}*/

void pasarFinalizado(infoESI * ESI){
	pthread_mutex_lock(&colaFinalizados);
	ESI = removerESI(listaListos, ESI->ID_ESI);
	pthread_mutex_unlock(&colaFinalizados);

	//Le seteo el estado en 2 (FINALIZADO)
	ESI->estado = finalizado;

	log_error(logger, "AGREGANDO A COLA DE FINALIZADOS");
	pthread_mutex_lock(&colaFinalizados);
	list_add(listaTerminados, (void*) ESI);
	pthread_mutex_unlock(&colaFinalizados);

	eliminarHiloDeConexion(ESI->socketESI);

}

infoESI * buscarESI(t_list * lista, int socket){

	bool socketIgual(infoESI* str){
		return (str->socketESI==socket);
	}

	return (infoESI *) list_find(lista,(void *)socketIgual);

}

infoESI * buscarESIxID(int id){
	bool igualID(infoESI* str){
		return (str->ID_ESI==id);
	}

	return (infoESI *) list_find(listaListos,(void *)igualID);
}

infoESI * removerESI(t_list * lista, int ID){

	bool IDIgual(infoESI* str){
		return (str->ID_ESI==ID);
	}

	log_info(logger, "Removiendo de LISTA al ID = %d", ID);

	return (infoESI *) list_remove_by_condition(lista,(void *)IDIgual);

}

//Funcion para hilo que planifica los distintos ESIS
void* planificar(){

	int corte = 1;
	while(corte){

		sem_wait(&esiListos);
		sem_wait(&semPlanificar);
		sem_wait(&sistemaEnEjecucion);
		sem_post(&sistemaEnEjecucion);
		log_warning(logger,"PLANIFICANDO!");
		ordenarColaReady();
		imprimirColaReady();
		//SACO EL PRIMERO
		int valor;
		sem_getvalue(&esiListos, &valor);

		//log_error(logger, "NRO DE ESIS LISTOS = %d \t VALOR DEL SEMAFORO DE ESIS LISTOS = %d", list_size(listaListos), valor);

		pthread_mutex_lock(&ESI_EXECUTOR);
		ESI_EJECUTANDO = (infoESI*) list_get(listaListos,0);
		ESI_EJECUTANDO->duracionDeRafaga++;
		ESI_EJECUTANDO->estado = 1;
		pthread_mutex_unlock(&ESI_EXECUTOR);

		ejecutarSentencia(ESI_EJECUTANDO);
	}

	return 0;

}





/*
esiEjecutando = malloc(sizeof(infoESI));
ultimoEsiEnEjecutar = malloc(sizeof(infoESI));


void alistarESI(int scktESI, t_list* lista){

	infoESI * str = removerESI(lista,scktESI);

	if(str != NULL){
		list_add(listaListos,str);
		log_info(logger,"ESI %d - READY",str->ID_ESI);
	} else {
		log_error(logger,"ESI %d NO ENCONTRADO",str->ID_ESI);
		perror("ESI NO ENCONTRADO");
		close(scktESI);
		exit(-1);
	}

}

void terminarESI(int scktESI, t_list* lista){

	infoESI * str = removerESI(lista,scktESI);

	if(str != NULL){
		list_add(listaTerminados,str);
		log_info(logger,"ESI %d - TERMINADO",str->ID_ESI);
	} else {
		log_error(logger,"ESI %d NO ENCONTRADO",str->ID_ESI);
		perror("ESI NO ENCONTRADO");
		close(scktESI);
		exit(-1);
	}

}

void liberarClave(char * clave){

	bool claveIgual(char* _clave){
		return (_clave==clave);
	}

	char * claveDestruida = list_remove_by_condition(listaClavesBloqueadas,(void *)claveIgual);

	free(claveDestruida);

}


infoESI * buscarEsiPorID (int ID_ESI){
	bool buscarESI(void* esi){
		return ((infoESI*)esi)->ID_ESI == ID_ESI;
	}
	return list_find(listaEsisTotal, buscarESI);



	void pasarEsiATerminado(int ID_ESI){

		bool buscarESI(void* esi){
				return ((infoESI*)esi)->ID_ESI == ID_ESI;
			}

		infoESI* esi = list_find(listaEsisTotal, buscarESI);

		switch(esi->estado){
			case listo:
			{
			list_remove_by_condition(listaListos,buscarESI);
				list_add(listaTerminados,esi);
				esi->estado = terminado
			;
			}

			break;
			case ejecutando:
			{
				free(esiEjecutando);
				list_add(listaTerminados,esiEjecutando);
				esi->estado = terminado;
			}
			break;
			case bloqueado:
			{
				list_remove_by_condition(listaBloqueados,buscarESI);
				list_add(listaTerminados,esi);
				esi->estado = terminado;
			}
			break;
		}

		free(esi);


		void bloquearEsi(int ID_ESI, char* claveDeBloqueo, int motivo){

				bool buscarEsi(infoESI* esi, ){
					return esi->ID_ESI == ID_ESI;
					}

					infoESI* esi = list_find(listaEsisTotal, buscarEsi);



					switch(esi->estado){
					case listo:
						esi->estado = bloqueado;

							t_bloqueado* esiBloqueado;
							esiBloqueado->ESI = esi;
							esiBloqueado->claveDeBloqueo = malloc(strlen(claveDeBloqueo));
							strcpy(claveDeBloqueo,esiBloqueado->claveDeBloqueo);
							esiBloqueado->motivo = motivo;

							list_remove_by_condition(listaListos,buscarEsi);
							list_add(listaBloqueados,esiBloqueado);

						break;
						case ejecutando:
						{
						modificarEstado* modificarEsi;
							modificarEsi->ESI = esi;
							modificarEsi->modificarEstado = bloquear;
							modificarEsi->claveDeBloqueo = malloc(strlen(claveDeBloqueo));
							strcpy(claveDeBloqueo,modificarEsi->claveDeBloqueo);
							modificarEsi->motivo = motivo;
						}
						break;



void matarESI(int socket){

	infoESI * str = buscarESI(listaEjecucion,socket);

	if(str != NULL){
		log_error(logger,"MATE ESI %d",str->ID_ESI);
		perror("Mate un ESI");
	} else {
		log_error(logger,"ESI %d NO ENCONTRADO",str->ID_ESI);
		perror("ESI no encontrado");
	}

	close(socket);
	//exit(-1);

}
*/






