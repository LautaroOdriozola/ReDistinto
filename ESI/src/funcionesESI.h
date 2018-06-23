#ifndef FUNCIONESESI_H_
#define FUNCIONESESI_H_

#include <socket/socket.h>
#include <socket/generarConfigs.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <parsi/parser.h>


// VARIABLES PARA ARCHIVO DE CONFIGURACION
char* COORDINADOR_IP;
int PUERTO_COORDINADOR;
char* PLANIFICADOR_IP;
int PUERTO_PLANIFICADOR;

int ID_ESI;
FILE* archivoAParsear;

t_log* logger;

int socketServerCoordinador;
int socketServerPlanificador;

FILE * abrirArchivoAParsear(char *);
//void parsearArchivo(FILE *);
void cargarConfigESI(t_config*);
void manejarOperacionDeParseo();


#endif /* FUNCIONESESI_H_ */
