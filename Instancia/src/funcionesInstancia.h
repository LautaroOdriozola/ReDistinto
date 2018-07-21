#ifndef FUNCIONESINSTANCIA_H_
#define FUNCIONESINSTANCIA_H_

#include <socket/socket.h>
#include <socket/generarConfigs.h>
#include <commons/config.h>
#include <commons/string.h>
#include <commons/log.h>
#include <commons/bitarray.h>
#include <commons/txt.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>

typedef struct _tablaDeEntrada{
	char* clave;
	int nroEntrada;
	int tamanioValor;
	bool variasEntradas;
	int cantidadDeReferencias;
} infoTablaDeEntradas;

typedef struct _infoPosicion{
	char* porcionDeValor;
} infoPosicion;

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
uint32_t TAMANIO_ENTRADA;
uint32_t CANTIDAD_ENTRADAS;
int punteroParaCircular;

t_list * tablaDeEntradas;
void* storage;
t_bitarray * bitArrayStorage;

pthread_mutex_t mutexTablaDeEntradas;
pthread_mutex_t mutexStorage;
pthread_mutex_t	mutexDump;

pthread_t hiloDump;

void cargarConfigInstancia(t_config*);
void iniciarEstructurasAdministrativasInstancia();
t_bitarray * crearBitArray(uint32_t);	// Creo bitArray a partir de la cantidad de entradas y la estructura
uint32_t escribirEnMemoria(infoTablaDeEntradas *, char*);

int devolverIndexParaAlmacenarValor(char*);
bool existeLugarContiguo(char*);
bool validarEspacioReal(char*);

int devolverPosicionLibreStorage();

//Funciones principales de operacion
void manejarOperacionSet();
void manejarOperacionStore();
void manejarOperacionBloquesLibres();
int calcularEntradasLibres();


void actualizarClaveAndValor(char*, char*);
void almacenarClaveAndValor(char*, char* );
void aplicarAlgoritmoDeReemplazo(char*);
int calcularCantidadEntradasAtomicas();
void manejarAlgoritmoLRU(char*);
void ordenarPorMayorReferencia();
void manejarAlgoritmoCircular(char*);
void sumarReferencia();
bool existeClaveEnTabla(char*);

void* realizarDump();
void funcionDump();

// Funciones auxiliares
void persistirClave(char*);
void chequearClave(char *);
infoTablaDeEntradas * crearStrParaAlmacenar(char*, int, int, bool);
infoTablaDeEntradas * getInfoTabla(char*);
char * getValor(infoTablaDeEntradas *);
int calcularCantidadDeEntradasAOcupar(char*);
int calcularCantidadPorNumero(int);
infoPosicion * crearStrValor(char*);
void borrarClaveDeTabla(infoTablaDeEntradas*);
int encontrarPosicion(infoTablaDeEntradas*);
void liberarInfoTabla(infoTablaDeEntradas*);
void liberarStorage();
void liberarMemoriaInstancia();
void laParcaDeInstancia(int);

#endif /* FUNCIONESINSTANCIA_H_ */
