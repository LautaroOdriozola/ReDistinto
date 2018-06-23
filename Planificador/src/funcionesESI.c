#include "funcionesESI.h"

void iniciarESI(int socket){

	infoESI * nuevoESI = malloc(sizeof(infoESI));

	nuevoESI->socketESI=socket;
	nuevoESI->rafaga=0;
	nuevoESI->estimacion=ESTIMACION_INICIAL;

	uint32_t id = recibirUint(socket);

	nuevoESI->ID_ESI=id;

	log_debug(logger,"Se ha creado el ESI N° %d",id);

	if(list_is_empty(listaEjecucion) && list_is_empty(listaListos)){
		log_info(logger,"ESI %d - EJECUTA directamente",id);
		list_add(listaEjecucion,nuevoESI);
		ejecutarSentencia(nuevoESI);
	} else {
		log_info(logger,"ESI %d - READY",id);
		list_add(listaListos,nuevoESI);
	}

}

void ejecutarSentencia(infoESI * str){
	log_error(logger, "Deberia decirle que ejecute sentencia pero todavia no lo hago.");
	//sendDeNotificacion(str->socketESI,PARSEAR_LINEA);
}

void manejarESI(int scktESI){

	iniciarESI(scktESI);

	while(1){

		uint32_t not = recvDeNotificacion(scktESI);

		switch(not){

			case OPERACION_EXITO:
				//alistarESI(scktESI,);
				//ejecutarProximoESI();
				break;

			case 0:
				//terminarESI(scktESI);
				//ejecutarProximoESI();
				break;

			default:
				matarESI(scktESI);

		}

	}

}

// No la uso
infoESI * buscarESI(t_list * lista, int socket){

	bool socketIgual(infoESI* str){
		return (str->socketESI==socket);
	}

	return (infoESI *) list_find(lista,(void *)socketIgual);

}

infoESI * removerESI(t_list * lista, int socket){

	bool socketIgual(infoESI* str){
		return (str->socketESI==socket);
	}

	return (infoESI *) list_remove_by_condition(lista,(void *)socketIgual);

}

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







