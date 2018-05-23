#include "Instancia.h"
#include "funcionesInstancia.h"


int main(int argc, char **argv) {
	logger = log_create(fileLog, "Instancia Logs", 0, 0);
	log_trace(logger, "Inicializando proceso Instancia");

	//Config para consola
	chequearParametros(argc,2);
	t_config* configuracionInstancia = generarTConfig(argv[1], 6);

	//Config para debug
	//t_config* configuracionInstancia = generarTConfig("Debug/instancia.ini", 6);

	cargarConfigInstancia(configuracionInstancia);


	socketServerCoordinador = conectarAServer(COORDINADOR_IP, PUERTO_COORDINADOR);
	puts("Me conecte con Coordinador! \n");
	realizarHandshake(socketServerCoordinador, ES_INSTANCIA, ES_COORDINADOR);
	log_trace(logger,"Me conecte con el Coordinador");

	iniciarEstructurasAdministrativasInstancia();		// Creo mallocs y bitArray para tablaEntradas y storage

	int corte = 1;

	while(corte){
		int nroOperacion = recvDeNotificacion(socketServerCoordinador);

		switch(nroOperacion){

			case OPERACION_GET:
				puts("No puedo hacer ningun GET!");
				puts("Comienzo a morir lentamente...");
				liberarMemoriaInstancia();
				break;

			case OPERACION_SET:
				manejarOperacionSet();
				break;

			case OPERACION_STORE:
				break;

			case 0:
				puts("Murio el coordinador");
				puts("Comienzo a morir lentamente...");
				liberarMemoriaInstancia();
				break;

			default:
				puts("No se puede realizar ninguna operacion.");
				puts("Comienzo a morir lentamente...");
				liberarMemoriaInstancia();

		} 	// Cierro switch

	}	//Cierro ciclo while


	liberarMemoriaInstancia();
	return EXIT_SUCCESS;
}
