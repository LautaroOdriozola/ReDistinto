#include "funcionesPlanificador.h"
#include "Planificador.h"

int main(int argc, char **argv) {
	logger = log_create(fileLog, "Coordinador Logs", 0, 0);
	log_trace(logger, "Inicializando proceso Planificador");

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
	log_trace(logger, "Planificador se conecto a Coordinador");
	puts("Me conecte con Coordinador!");

	//Dentro de esta funcion manejo el select con sus nuevas conexiones
	manejarSelect();

	log_destroy(logger);

	return EXIT_SUCCESS;
}
