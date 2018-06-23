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
    int rafaga;
    int estimacion;
} infoESI;


// VARIABLES PARA ARCHIVO DE CONFIG
int PUERTO_ESCUCHA;
char* ALGORITMO_PLANIFICACION;
int ESTIMACION_INICIAL;
char* COORDINADOR_IP;
int PUERTO_COORDINADOR;
char ** CLAVES_BLOQUEADAS;

t_log* logger;
int socketServerCoordinador;	// PARA USUARIO
int socketListener,socketMaximo;	 // PARA SERVER

t_list * listaListos;
t_list * listaEjecucion;
t_list * listaTerminados;
t_list * listaBloqueados;
t_list * listaClavesBloqueadas;



void levantarConsolaPlanificador();
void cargarConfigPlanificador(t_config*);
void * atenderNotificacion(void*);
void manejarConexiones();
void chequeoSocket(int);
void iniciarEstructurasAdministrativasPlanificador();
void liberarMemoriaPlanificador();


#endif /* FUNCIONESPLANIFICADOR_H_ */
