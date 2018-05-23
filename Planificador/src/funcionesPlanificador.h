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
fd_set socketClientes, socketClientesAuxiliares; // PARA SERVER



void levantarConsolaPlanificador();
void cargarConfigPlanificador(t_config*);
void atenderConexion(int);
void atenderNotificacion(int);
void manejarSelect();

#endif /* FUNCIONESPLANIFICADOR_H_ */
