#include "funcionesPlanificador.h"

int main(int argc, char **argv) {
	// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/Escritorio/tp-2018-1c-EnMiCompuCompilaba/socket/Debug/
	// ARGUMENTOS:
	// ./Planificador planificador.ini

	char* fileLog = "PlanificadorLogs.txt";
	logger = log_create(fileLog, "Planificador", 1, 0);
	log_info(logger, "Inicializando proceso Planificador");

	// Config para consola
	chequearParametros(argc,2);
	t_config* configuracionPlanificador= generarTConfig(argv[1], 6);

	//Config para debug
	//t_config* configuracionPlanificador = generarTConfig("Debug/planificador.ini", 6);

	cargarConfigPlanificador(configuracionPlanificador);

	iniciarEstructurasAdministrativasPlanificador();

	//Crear hilo para la consola con funcion::
	//levantarConsolaPlanificador();

	//Me conecto a Coordinador
	socketServerCoordinador = conectarAServer(COORDINADOR_IP, PUERTO_COORDINADOR);
	realizarHandshake(socketServerCoordinador, ES_PLANIFICADOR, ES_COORDINADOR);
	log_info(logger, "Planificador se conecto a Coordinador");

	// Genero servidor para los ESI
	log_trace(logger, "Estoy a la espera de conexiones");
	// Genero el socket servidor
	socketListener = iniciarServidor(PUERTO_ESCUCHA);

	manejarConexiones();

	liberarMemoriaPlanificador();
	return EXIT_SUCCESS;
}
