#include "funcionesInstancia.h"


int main(int argc, char **argv) {

	char * fileLog = "Instancia Logs.txt";
	logger = log_create(fileLog, "Instancia Logs", 1, 0);
	log_info(logger, "Inicializando proceso Instancia");

	//Config para consola
	chequearParametros(argc,2);
	t_config* configuracionInstancia = generarTConfig(argv[1], 6);

	//Config para debug
	//t_config* configuracionInstancia = generarTConfig("Debug/instancia.ini", 6);

	cargarConfigInstancia(configuracionInstancia);


	socketServerCoordinador = conectarAServer(COORDINADOR_IP, PUERTO_COORDINADOR);
	realizarHandshake(socketServerCoordinador, ES_INSTANCIA, ES_COORDINADOR);
	log_info(logger,"Me conecte con el Coordinador");

	iniciarEstructurasAdministrativasInstancia();		// Creo mallocs y bitArray para tablaEntradas y storage

	int corte = 1;

	while(corte){
		int nroOperacion = recvDeNotificacion(socketServerCoordinador);

		switch(nroOperacion){

			case OPERACION_GET:
				log_info(logger,"No puedo hacer ningun GET!");
				log_info(logger,"Comienzo a morir lentamente...");
				liberarMemoriaInstancia();
				break;

			case OPERACION_SET:
				manejarOperacionSet();
				break;

			case OPERACION_STORE:
				break;

			case 0:
				log_info(logger,"Murio el COORDINADOR");
				log_info(logger,"Comienzo a morir lentamente...");
				liberarMemoriaInstancia();
				break;

			default:
				log_info(logger,"No se puede realizar ninguna operacion.");
				log_info(logger,"Comienzo a morir lentamente...");
				liberarMemoriaInstancia();

		} 	// Cierro switch

	}	//Cierro ciclo while


	liberarMemoriaInstancia();
	return EXIT_SUCCESS;
}
