#ifndef GENERICASCOORDINADOR_H_
#define GENERICASCOORDINADOR_H_


#include <socket/socket.h>
#include <socket/generarConfigs.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

typedef struct _infoAlgoritmoDistribucion{
	int socketInstancia;		//Para poder mensajear
	t_list* clavesTomadas;			//lista de claves para una instancia
	int cantidadEntradasLibres;	//Para algoritmo LSU
} infoAlgoritmoDistribucion;

typedef struct _infoESI{
	int socket;
	int ID_ESI;
} infoESI;

//Estructura para poder matar los hilos al final o si se cayo algun socket
typedef struct _infoHilos{
	int socket;
	pthread_t hiloAtendedor;
} infoHilos;

// Variable para poder diferenciar el socket del planificador
int socketPlanificador;
int socketListener;
t_log* logger;

// VARIABLES PARA ESTRUCTURAS ADMINISTRATIVAS
t_list * listaDeHilos;
t_list * instanciasConectadas;	// para manejar las instancias para los algoritmos
t_list * ESIsConectados;	// para manejar los ESIS
t_log* operaciones; // log de operaciones

// SEMAFOROS MUTEX
pthread_mutex_t instConectadas;
pthread_mutex_t EsisConectados;
pthread_mutex_t clavesTomadas;
pthread_mutex_t mutexHilos;


// VARIABLES PARA ARCHIVO DE CONFIGURACION
int PUERTO_ESCUCHA;
char* ALGORITMO;
int CANTIDAD_ENTRADAS;
int	TAMANIO_ENTRADA;
int RETARDO;

void chequeoSocket(int);
void cargarConfigCoordinador(t_config*);
void iniciarEstructurasAdministrativasCoordinador();

void loguearRespuestaGet(int, char *);
void loguearRespuestaSet(int,char*,char*);
void loguearRespuestaStore(int,char*);

void cerrandoSocketsInstancias();
void cerrandoSocketsESIS();
void liberarClavesDeInstancias();
void eliminarHiloDeConexion(int);
void liberarInstancias(infoAlgoritmoDistribucion*);
void liberarClaves(t_list *);
void liberarESIs(infoESI*);
void liberarHilo(infoHilos *);
void liberandoHilos();
void laParca(int);
void liberarMemoriaCoordinador();

#endif /* GENERICASCOORDINADOR_H_ */
