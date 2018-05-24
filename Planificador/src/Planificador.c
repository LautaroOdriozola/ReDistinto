#include "funcionesPlanificador.h"
#include "Planificador.h"

int main(int argc, char **argv) {
	logger = log_create(fileLog, "Planificador Logs", 1, 0);
	log_info(logger, "Inicializando proceso Planificador");

	// Config para consola
	chequearParametros(argc,2);
	t_config* configuracionPlanificador= generarTConfig(argv[1], 6);

	//Config para debug
	//t_config* configuracionPlanificador = generarTConfig("Debug/planificador.ini", 6);

	cargarConfigPlanificador(configuracionPlanificador);

	//Crear hilo para la consola con funcion::
	//levantarConsolaPlanificador();

	//Me conecto a Coordinador
	socketServerCoordinador = conectarAServer(COORDINADOR_IP, PUERTO_COORDINADOR);
	realizarHandshake(socketServerCoordinador, ES_PLANIFICADOR, ES_COORDINADOR);
	log_info(logger, "Planificador se conecto a Coordinador");

	//Dentro de esta funcion manejo el select con sus nuevas conexiones
	manejarSelect();

	log_destroy(logger);

	return EXIT_SUCCESS;
}
