#ifndef GENERICASCOORDINADOR_H_
#define GENERICASCOORDINADOR_H_


#include <socket/socket.h>
#include <socket/generarConfigs.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

int socketListener;
t_log* logger;
// Variable para poder diferenciar el socket del planificador
int socketPlanificador;


// VARIABLES PARA ESTRUCTURAS ADMINISTRATIVAS
t_list * instanciasConectadas;	// para manejar las instancias para los algoritmos
/*
 * UN SEMAFORO PARA LAS INSTANCIAS CONECTADAS
 * MUTEX
 */

t_list * ESIsconectados;

t_list * clavesBloqueadas;	// para saber claves bloqueadas
/*
 * UN SEMAFORO PARA LAS CLAVES BLOQUEADAS
 * MUTEX
 *
 */

t_list * operaciones; // lista para log de operaciones

// VARIABLES PARA ARCHIVO DE CONFIGURACION
int PUERTO_ESCUCHA;
char* ALGORITMO;
int CANTIDAD_ENTRADAS;
int	TAMANIO_ENTRADA;
int RETARDO;

void chequeoSocket(int);
void cargarConfigCoordinador(t_config*);
void iniciarEstructurasAdministrativasCoordinador();
void liberarMemoriaCoordinador();

#endif /* GENERICASCOORDINADOR_H_ */
