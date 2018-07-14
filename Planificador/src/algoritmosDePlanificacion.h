/*
 * algoritmosDePlanificacion.h
 *
 *  Created on: 30 jun. 2018
 *      Author: utnso
 */

#ifndef ALGORITMOSDEPLANIFICACION_H_
#define ALGORITMOSDEPLANIFICACION_H_

#include "funcionesPlanificador.h"
//#include "funcionesESI.h"

void imprimirColaReady();
void ordenarColaReady();
void ordenarPorSjf_Sd();
void ordenarPorSjf_Cd();
void estimarListos();
bool compararEstimaciones(infoESI*, infoESI*);
void estimarRafaga(infoESI *);


#endif /* ALGORITMOSDEPLANIFICACION_H_ */
