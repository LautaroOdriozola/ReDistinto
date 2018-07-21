#include "funcionesConsola.h"

uint32_t countAmmount = 8;

command commands[] = {
	{ 1,"pausar"},
	{ 2,"continuar"},
	{ 3,"bloquear"},
	{ 4,"desbloquear"},
	{ 5,"listar"},
	{ 6,"kill"},
	{ 7,"status"},
	{ 8,"deadlock"}
};

//Consola PLANIFICADOR
void* levantarConsolaPlanificador(){
	printf("Consola Iniciada. Ingrese una opcion \n");
	log_trace(logger, "Se inicializo la consola");

	size_t maxSize = 100*sizeof(char);
		while(1) {
			puts("Esperando comando...");
			printf(">>");
			getline(&commandChar, &maxSize, stdin);
			uint32_t recvdSize = strlen(commandChar);
			commandChar[recvdSize-1] = '\0';

			if(string_length(commandChar)==0){
				log_warning(logger,"Comando no reconocido");
			}  else {

				if(strcmp(commandChar, "exit")==0){
					free(commandChar);
					pthread_detach(pthread_self());
					break;
				}

				analizarComando(commandChar);
			}
	 }

	return 0;
}

void analizarComando(char* cmd){

	char** disarmedCmd = string_split(cmd, " ");
	char* function = disarmedCmd[0];
	uint32_t commandNumber = 99;
	command auxCommand;
	int count;
	for(count = 0; count < countAmmount; count++){
		auxCommand = commands[count];
		if(strcmp(function, auxCommand.nombre)==0){
			commandNumber = auxCommand.flag;
			break;
		}
	}

	ejecutarComando(commandNumber,disarmedCmd);
	liberarRutaDesarmada(disarmedCmd);
}

void liberarRutaDesarmada(char** ruta){
	uint32_t cont=0;
	for(;ruta[cont]!=NULL;cont++){
		free(ruta[cont]);
	}
	free(ruta);
}

void ejecutarComando(uint32_t nro, char** parametros){

	switch(nro){
		case 1:
			pausar();
			break;
		case 2:
			continuar();
			break;
		case 3:
			break;
		case 4:
			desbloquear(parametros);
			break;
		case 5:
			listar(parametros);
			break;
		case 6:
			kill_ESI(parametros);
			break;
		case 7:
			break;
		case 8:
			log_debug(logger,"EJECUTANDO COMANDO DE DEADLOCK!");
			deadlock();
			break;

		default:{
			log_warning(logger,"Comando no reconocido");
		}

	}


}

bool estaPausado(){
	int valor;
	sem_getvalue(&sistemaEnEjecucion, &valor);
	if( valor > 0){
		return false;
	} else{
		return true;
	}
}

void pausar(){
	if(estaPausado()){
		log_warning(logger, "EL SISTEMA YA ESTABA PAUSADO");
	} else {
		log_warning(logger, "SISTEMA PAUSADO");
		sem_wait(&sistemaEnEjecucion);
	}
}

void continuar(){
	if(estaPausado()){
		log_warning(logger, "VUELVE A CONTINUAR EL SISTEMA");
		sem_post(&sistemaEnEjecucion);
	} else{
		log_warning(logger, "EL SISTEMA ESTABA EJECUTANDO");
	}
}

void desbloquear(char ** param){
	char * clave = param[1];
	log_warning(logger, "CLAVE A DESBLOQUEAR POR CONSOLA = %s", clave);
	int largo = list_size(listaBloqueados);
	int i;

	for(i=0; i<largo; i++){
		infoESIBloqueado * data = (infoESIBloqueado*) list_get(listaBloqueados, i);

		if(data != NULL){
			if((strcmp(data->claveCausante, clave) == 0)){
				desbloquearClave(clave);
				//sem_post(&esiListos);
				//sem_post(&semPlanificar);
			}
		}
	}

}

void listar(char ** param){
	char * clave = param[1];

	log_warning(logger,"MOSTRANDO ESIS ESPERANDO POR LA LIBERACION DE LA CLAVE = %s", clave);

	int largo = list_size(listaBloqueados);

	log_trace(logger, "CANTIDAD DE ESIS BLOQUEADOS = %d", largo);
	int i;

	for(i=0; i<largo; i++){
		infoESIBloqueado * data = (infoESIBloqueado*) list_get(listaBloqueados, i);
		if((strcmp(data->claveCausante, clave) == 0)){
			printf("\t %d \n", data->ESI->ID_ESI);
		}
	}
}

int cantParam(char ** com){
	int i=0;
	while(com[i] != NULL){
		i++;
	}
	return i;
}

bool chequearParamCom(char ** com, int cantMin, int cantMax){
	int x = cantParam(com);
	return cantMin <= x && x <= cantMax;
}









