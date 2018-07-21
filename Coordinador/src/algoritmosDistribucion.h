/*
 * algoritmosDistribucion.h
 *
 *  Created on: 8 jul. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOSDISTRIBUCION_H_
#define ALGORITMOSDISTRIBUCION_H_

#include "genericasCoordinador.h"


infoAlgoritmoDistribucion* getInstanciaEquitativeLoad();
infoAlgoritmoDistribucion* getInstanciaLeastSpaceUsed();
bool existeEspacioLibreIgual(infoAlgoritmoDistribucion *, infoAlgoritmoDistribucion *);
infoAlgoritmoDistribucion * encontrarInstanciaConMayorEspacioLibre();
void ordenarListaDeInstanciasDeMayorToMenor();

#endif /* ALGORITMOSDISTRIBUCION_H_ */
