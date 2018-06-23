#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include "genericasCoordinador.h"

void manejadorDeHilos();
void * atenderNotificacion(void*);
void manejarInstancia(int);
void manejarESI(int);
void manejarOperacionGet(int);
void manejarOperacionSet(int);
infoAlgoritmoDistribucion * elegirInstancia(char*);
void agregarInstancia(int);
void agregarESI(int, int);
bool existeClave(char*);
bool existeClaveEnLista(t_list *, char*);
int buscarIdEsi(int);
infoAlgoritmoDistribucion* getInstanciaEquitativeLoad(char*);
void agregarClave(int, char*);


typedef struct _infoAlgoritmoDistribucion{
	int socketInstancia;		//Para poder mensajear
	t_list* clavesTomadas;			//lista de claves para una instancia
	int cantidadEntradasEscritas;	//Para algoritmo LSU
} infoAlgoritmoDistribucion;

typedef struct _infoESI{
	int socket;
	int ID_ESI;
} infoESI;

#endif /* ESTRUCTURAS_H_ */
