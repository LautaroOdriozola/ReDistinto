#include "funcionesESI.h"


int main(int argc, char **argv){

	// ARGUMENTOS:
	// ./ESI ESI.ini script.esi ID_ESI

	char* fileLog = "ESILogs.txt";
	logger = log_create(fileLog, "ESI", 1, 0);
	log_info(logger, "Inicializando proceso ESI");

	// Por ahora solo le estamos mandando el archivo de configuracion,el .esi y el ID, por eso el 4
	// Config para consola
	chequearParametros(argc,4);
	t_config* configuracionESI = generarTConfig(argv[1], 4);

	//Config para debug
	//t_config* configuracionESI = generarTConfig("Debug/ESI.ini", 4);

	cargarConfigESI(configuracionESI);

	listaDeInstrucciones = list_create();
	INICIO_DE_INSTRUCCION = 0;

	//Armo el ID_ESI
	char* ptr;
	ID_ESI = strtol(argv[3], &ptr, 10);

	log_debug(logger, "SOY EL ESI NRO = %d", ID_ESI);

	archivoAParsear = abrirArchivoAParsear(argv[2]);
	//Imprimo script a ejecutar y cuento lineas del script
	contarLineas(archivoAParsear);

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
	// Envio el nro (ID)
	sendDeNotificacion(socketServerPlanificador, (uint32_t) ID_ESI);

	cantidadDeInstrucciones = list_size(listaDeInstrucciones);
	// Envio cantidad de instrucciones a parsear
	sendDeNotificacion(socketServerPlanificador, (uint32_t) cantidadDeInstrucciones);

	//Le mando al coordinador que ESI soy
	log_info(logger,"Envio ID de ESI al COORDINADORR.");
	sendDeNotificacion(socketServerCoordinador, (uint32_t) ID_ESI);

	int corte = 1;
	while(corte){
		int nroOperacion = recvDeNotificacion(socketServerPlanificador);

		switch(nroOperacion){


			case PARSEAR_LINEA:
				manejarOperacionDeParseo();
				break;

			case ABORTAR_ESI:
				log_error(logger,"DESPIDIENDOME DE ESTE MUNDO...");
				log_error(logger,"MURIENDO LENTAMENTE..");
				liberarMemoriaESI();
				break;

			case 0:
				log_error(logger,"Murio el PLANIFICADOR");
				log_error(logger,"Comienzo a morir lentamente...");
				liberarMemoriaESI();
				break;

			default:
				log_error(logger,"No se puede realizar ninguna operacion.");
				log_error(logger,"Comienzo a morir lentamente...");
				liberarMemoriaESI();


		}	//Cierro switch

	}	//Cierro while


	liberarMemoriaESI();
	return EXIT_SUCCESS;
}
