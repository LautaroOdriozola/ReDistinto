#ifndef GENERARCONFIGS_H_
#define GENERARCONFIGS_H_

#include "socket.h"
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>


int existeArchivo(char*);
void chequearParametros(int, int);
void chequearExistenciaArchivo(char*);
void chequearCantidadDeKeys(t_config*, int);
t_config* generarTConfig(char*, int);


#endif /* GENERARCONFIGS_H_ */
