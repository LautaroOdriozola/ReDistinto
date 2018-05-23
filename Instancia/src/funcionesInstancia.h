#ifndef FUNCIONESINSTANCIA_H_
#define FUNCIONESINSTANCIA_H_

#include <socket/socket.h>
#include <socket/generarConfigs.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

typedef struct _tablaDeEntrada{
	char* clave;
	int nroEntrada;
	int tamanioValor;
	bool unicaEntrada;
} infoTablaDeEntradas;

// VARIABLES ARCHIVO DE CONFIG
char* COORDINADOR_IP;
int PUERTO_COORDINADOR;
char* ALGORITMO_REEMPLAZO;
char* PATH_MONTAJE;
char* NOMBRE_INSTANCIA;
int INTERVALO_DUMP;

t_log* logger;
int socketServerCoordinador;

// VARIABLES PARA CREAR ESTRUCTURAS ADMINISTRATIVAS
int TAMANIO_ENTRADA;
int	CANTIDAD_ENTRADAS;
void* tablaDeEntradas;
t_bitarray * bitArrayTablaDeEntradas;
void* storage;

void cargarConfigInstancia(t_config*);
void iniciarEstructurasAdministrativasInstancia();
t_bitarray * crearBitArray(int);	// Creo bitArray a partir de la cantidad de entradas y la estructura
void escribirEnMemoria(infoTablaDeEntradas *, char*);
int devolverPosicionLibre();
void manejarOperacionSet();
infoTablaDeEntradas * crearStrParaAlmacenar(char*, int, int, bool);
int calcularCantidadDeEntradasAOcupar(char*);
void liberarMemoriaInstancia();


#endif /* FUNCIONESINSTANCIA_H_ */