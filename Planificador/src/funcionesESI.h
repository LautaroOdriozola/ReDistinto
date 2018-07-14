/*
 * funcionesESI.h
 *
 *  Created on: 21 jun. 2018
 *      Author: utnso
 */

#ifndef FUNCIONESESI_H_
#define FUNCIONESESI_H_

#include "funcionesPlanificador.h"
#include "funcionesConsola.h"

void iniciarESI(int);
void manejarESI(int);
void ejecutarSentencia(infoESI *);

void eliminarHiloDeConexion(int);
void ejecucionDeESIExitosa();
void bloquearESI(infoESI*, char *);
void bloquearClave(char*,int);
void pasarESIABloqueado(char*, int );
void pasarESIAListo(infoESI *);
void pasarESIAFinalizado(infoESI *);
void eliminarDeColaActual(infoESI *);

void pasarFinalizado(infoESI *);
infoESI * buscarESI(t_list *, int);
infoESI * buscarESIxID(int);
infoESI * removerESI(t_list *, int);

void* planificar();
/*
void terminarESI(int, t_list*);
void alistarESI(int,t_list*);
void matarESI(int);
infoESI * buscarESI(t_list *,int);
infoESI * removerESI(t_list *,int);
void pasarEsiATerminado (int);
void bloquearEsi (int, char*, int);
*/

#endif /* FUNCIONESESI_H_ */
