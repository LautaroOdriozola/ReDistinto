/*
 * funcionesConsola.h
 *
 *  Created on: 8 jul. 2018
 *      Author: utnso
 */

#ifndef FUNCIONESCONSOLA_H_
#define FUNCIONESCONSOLA_H_

#include "funcionesPlanificador.h"
#include "funcionesESI.h"

//Variable global para los comandos de consola
char * commandChar;

typedef struct {
	int flag;
	char *nombre;     /* Nombre de la funcion */
} command;

void* levantarConsolaPlanificador();
void analizarComando(char *);
void liberarRutaDesarmada(char**);
void ejecutarComando(uint32_t,char **);
bool estaPausado();

void pausar();
void continuar();
void desbloquear(char **);
void listar(char **);

int cantParam(char**);
bool chequearParamCom(char**, int, int);


#endif /* FUNCIONESCONSOLA_H_ */
