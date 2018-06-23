/*
 * funcionesESI.h
 *
 *  Created on: 21 jun. 2018
 *      Author: utnso
 */

#ifndef FUNCIONESESI_H_
#define FUNCIONESESI_H_

#include "funcionesPlanificador.h"

void manejarESI(int);
void iniciarESI(int);
void ejecutarSentencia(infoESI *);
void terminarESI(int, t_list*);
void alistarESI(int,t_list*);
void matarESI(int);
infoESI * buscarESI(t_list *,int);
infoESI * removerESI(t_list *,int);


#endif /* FUNCIONESESI_H_ */
