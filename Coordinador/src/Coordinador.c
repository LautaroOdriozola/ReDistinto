#include "estructuras.h"

int main(int argc, char **argv){

	// export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:~/Escritorio/tp-2018-1c-EnMiCompuCompilaba/socket/Debug/
	// ARGUMENTOS:
	// ./Coordinador coordinador.ini

	char* fileLog = "CoordinadorLogs.txt";
	logger = log_create(fileLog, "Coordinador", 1, 0);
	log_info(logger, "Inicializando proceso Coordinador");

	// Config para consola
	chequearParametros(argc,2);
	t_config* configuracionCoordinador = generarTConfig(argv[1], 5);

	//Config para debug
	//t_config* configuracionCoordinador = generarTConfig("Debug/coordinador.ini", 5);

	cargarConfigCoordinador(configuracionCoordinador);

	iniciarEstructurasAdministrativasCoordinador();

	log_trace(logger, "Estoy a la espera de conexiones");
	// Genero el socket servidor
	socketListener = iniciarServidor(PUERTO_ESCUCHA);

	//Acepto conexiones y manejo sus respuestas
	manejadorDeHilos();

	liberarMemoriaCoordinador();
	return EXIT_SUCCESS;
}
