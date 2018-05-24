#include "ESI.h"
#include "funcionesESI.h"


int main(int argc, char **argv){

	logger = log_create(fileLog, "ESI Logs", 1, 0);
	log_info(logger, "Inicializando proceso ESI");

	// Por ahora solo le estamos mandando el archivo de configuracion y el .esi, por eso el 3
	// Config para consola
	chequearParametros(argc,3);
	t_config* configuracionESI = generarTConfig(argv[1], 4);

	//Config para debug
	//t_config* configuracionESI = generarTConfig("Debug/ESI.ini", 4);

	cargarConfigESI(configuracionESI);

	// conexion con coordinador
	socketServerCoordinador = conectarAServer(COORDINADOR_IP, PUERTO_COORDINADOR);
	realizarHandshake(socketServerCoordinador, ES_ESI, ES_COORDINADOR);
	log_info(logger, "Me conecte con Coordinador!");


	//conexion con planificador
	socketServerPlanificador = conectarAServer(PLANIFICADOR_IP, PUERTO_PLANIFICADOR);
	realizarHandshake(socketServerPlanificador, ES_ESI, ES_PLANIFICADOR);
	log_info(logger,"Me conecte con Planificador!");

/*
	puts("Parseo un poquito!");
	FILE * ejemploParser = abrirArchivoAParsear(argv[2]);
	parsearArchivo(ejemploParser);
*/
	log_destroy(logger);
	return EXIT_SUCCESS;
}
