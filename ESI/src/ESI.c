#include "ESI.h"
#include "funcionesESI.h"


int main(int argc, char **argv){

	// ARGUMENTOS:
	// ./ESI ESI.ini script.esi ID_ESI

	logger = log_create(fileLog, "ESI Logs", 1, 0);
	log_info(logger, "Inicializando proceso ESI");

	// Por ahora solo le estamos mandando el archivo de configuracion,el .esi y el ID, por eso el 4
	// Config para consola
	chequearParametros(argc,4);
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

	//Le mando al planificador que ESI soy.
	log_info(logger,"Envio ID de ESI al PLANIFICADOR.");
	ID_ESI = argv[3];
	// Envio el nro (ID)
	sendDeNotificacion(socket, ID_ESI);


	archivoAParsear = abrirArchivoAParsear(argv[2]);

	int corte = 1;
	while(corte){
		int nroOperacion = recvDeNotificacion(socketServerPlanificador);

		switch(nroOperacion){


			case PARSEAR_LINEA:
				//manejarOperacionDeParseo();
				break;

			case 0:
				log_info(logger,"Murio el PLANIFICADOR");
				log_info(logger,"Comienzo a morir lentamente...");
				//liberarMemoriaESI();
				break;

			default:
				log_info(logger,"No se puede realizar ninguna operacion.");
				log_info(logger,"Comienzo a morir lentamente...");
				//liberarMemoriaESI();




		}	//Cierro switch

	}	//Cierro while


	//liberarMemoriaESI();
	log_destroy(logger);
	return EXIT_SUCCESS;
}
