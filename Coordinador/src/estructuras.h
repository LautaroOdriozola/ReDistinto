#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include "genericasCoordinador.h"

void manejadorDeHilos();
void * atenderNotificacion(void*);
void manejarInstancia(int);
void manejarESI(int);
void manejarOperacionGet(int);
void manejarOperacionSet(int);
int retornarInstancia();
void agregarInstancia(int);
int getInstanciaEquitativeLoad();

typedef struct _infoAlgoritmoDistribucion{
	int socketInstancia;
	int espacioUtilizado;
} infoAlgoritmoDistribucion;

#endif /* ESTRUCTURAS_H_ */
