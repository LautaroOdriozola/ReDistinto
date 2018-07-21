#ifndef FUNCIONESPLANIFICADOR_H_
#define FUNCIONESPLANIFICADOR_H_

#include <socket/socket.h>
#include <socket/generarConfigs.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

typedef struct _ESI{
    int ID_ESI;
    int socketESI;
    float duracionDeRafaga;
    float estimacion;
    int cantidadDeInstrucciones;
    int estado;
    int espera;
    int posicionEnLista;
} infoESI;

// Estructura para lista con claves bloqueadas
typedef struct _infoclaveBloqueada{
	char* claveBloqueada;
	int ID_ESI;
} infoClaveBloqueada;

//Estructura para lista con ESIS bloqueados. Necesito guardar la clave por la cual se bloquea el ESI
typedef struct _infoESIBloqueado{
	char * claveCausante;
	infoESI * ESI;
}infoESIBloqueado;


//Estructura para poder matar los hilos al final o si se cayo algun socket
typedef struct _infoHilos{
	int socket;
	pthread_t hiloAtendedor;
} infoHilos;

enum estado {
	listo = 0,
	ejecutando = 1,
	finalizado = 2,
	bloqueado = 3,
};




// VARIABLES PARA ARCHIVO DE CONFIG
int PUERTO_ESCUCHA;
char* ALGORITMO_PLANIFICACION;
int ALFA_PLANIFICACION;
int ESTIMACION_INICIAL;
char* COORDINADOR_IP;
int PUERTO_COORDINADOR;
char ** CLAVES_BLOQUEADAS;


/************/
int posicionListaListos;	// variable para algoritmo FIFO
/************/

t_log* logger;
int socketServerCoordinador;	// PARA USUARIO
int socketListener,socketMaximo;	 // PARA SERVER
fd_set socketClientes, socketClientesAuxiliares; // PARA SERVER

infoESI * ESI_EJECUTANDO;

//HILOS
pthread_t hiloConsola;
pthread_t hiloCoordinador;
pthread_t hiloPlanificador;

//Semaforos
pthread_mutex_t mutexHilos;
pthread_mutex_t colaReady;
pthread_mutex_t colaBloqueados;
pthread_mutex_t colaFinalizados;
pthread_mutex_t clavesBloqueadas;
pthread_mutex_t ESI_EXECUTOR;

sem_t sistemaEnEjecucion;
sem_t semPlanificar;
sem_t esiListos;


t_list * hilosParaConexiones;
t_list * listaListos;
t_list * listaTerminados;
t_list * listaBloqueados; 		//infoESIBloqueado
t_list * listaClavesBloqueadas; //infoClaveBloqueada



void* manejarCoordinador();
bool existeClaveEnLista(char*);
void manejarRespuestaAgregarClave(int);
void manejarRespuestaEliminarClave(int);
int agregarClave(char*,int);
int eliminarClave(char*);
void desbloquearClavesDeESI(infoESI *);
void desbloquearClave(char*);
void eliminarClaveBloqueada(char*);
infoESI* eliminarESIBloqueado(infoESI*);
bool existeESIBloqueadoPorClave(char*);
infoESI * encontrarESIConClaveBloqueada(char *);

void cargarConfigPlanificador(t_config*);
void * atenderNotificacion(void*);
void atenderConexion(int);
void chequeoSocket(int);
void manejarConexiones();

void iniciarHiloConsola();
void iniciarHiloQueAtiendeAlCoordinador();
void iniciarHiloQuePlanifica();
void iniciarEstructurasAdministrativasPlanificador();
void cerrandoSockets(t_list *);
void liberarESI(infoESI*);
void liberarESISDeBloqueados(void *);
void liberarClave(infoClaveBloqueada*);
void liberarHilo(infoHilos *);
void parcaDePlanificador(int);
void liberarMemoriaPlanificador();

t_list * filtrarBloqueadosPorClave(char* clave);
t_list * filtrarClavesBloqueadasPorID(int);
bool existeDeadlock(char*, t_list *);
void kill_ESI(char **);
void deadlock();


#endif /* FUNCIONESPLANIFICADOR_H_ */
