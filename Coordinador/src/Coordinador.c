#include "Coordinador.h"
#include "estructuras.h"

int main(int argc, char **argv){

	logger = log_create(fileLog, "Coordinador Logs", 1, 0);
	log_info(logger, "Inicializando proceso Coordinador");

	// Config para consola
	chequearParametros(argc,2);
	t_config* configuracionCoordinador = generarTConfig(argv[1], 5);

	//Config para debug
	//t_config* configuracionCoordinador = generarTConfig("Debug/coordinador.ini", 5);

	cargarConfigCoordinador(configuracionCoordinador);

	log_info(logger, "Estoy a la espera de conexiones");
	// Genero el socket servidor
	socketListener = iniciarServidor(PUERTO_ESCUCHA);

	iniciarEstructurasAdministrativasCoordinador();

	//Acepto conexiones y manejo sus respuestas
	manejadorDeHilos();

	liberarMemoriaCoordinador();
	return EXIT_SUCCESS;
}
