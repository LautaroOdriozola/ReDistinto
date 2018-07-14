#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

#include "genericasCoordinador.h"
#include "algoritmosDistribucion.h"

void manejadorDeHilos();
void * atenderNotificacion(void*);
void manejarInstancia(int);
void manejarESI(int);
void manejarOperacionGet(int);
void manejarOperacionSet(int);
void manejarOperacionStore(int);


infoAlgoritmoDistribucion * elegirInstancia(char*);
void agregarInstancia(int);
void agregarESI(int, int);
bool existeClave(char*);
bool existeClaveEnLista(t_list *, char*);
int buscarIdEsi(int);



void agregarClave(int, char*);
void eliminarClave(int, char*);
infoAlgoritmoDistribucion * encontrarInstanciaConClave(char*);

#endif /* ESTRUCTURAS_H_ */
