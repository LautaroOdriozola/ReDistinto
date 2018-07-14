#ifndef SOCKET_H_
#define SOCKET_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <signal.h>
#include <netdb.h>
#include <math.h>
#include <semaphore.h>
#include <readline/readline.h>
#include <readline/history.h>

/* 			PROTOCOLO DE CONEXIONES	*/
#define ES_COORDINADOR 1
#define ES_PLANIFICADOR 2
#define ES_ESI 3
#define ES_INSTANCIA 4

// TAMAÑO MAXIMO DE UNA CLAVE
#define TAMANIO_MAX_CLAVE 40

// Protocolo envio de operaciones basicas
#define OPERACION_SET 101
#define OPERACION_GET 102
#define OPERACION_STORE 103

//Protocolo para cachear errores
#define ERROR_TAMANIO_CLAVE 104
#define ERROR_CLAVE_NO_IDENTIFICADA 105
#define ERROR_COMUNICACION 106
#define ERROR_CLAVE_INACCESIBLE	 107
#define ERROR_CLAVE_NO_BLOQUEADA 108
#define ERROR_DE_INSTANCIA 109


/*			PROTOCOLO DE ENVIO DE MENSAJES 			*/
//		ENVIO DE INTS EN CADA TIPO DE MENSAJE
#define	DATOS_ADMINISTRATIVOS 110		// Para Coordinador
#define OPERACION_EXITO 111
#define INFO_ESI 112
#define OPERACION_BLOQUES_LIBRES 113

#define PARSEAR_LINEA 114
#define REVISAR_CLAVE 115

#define BLOQUEAR_ESI 116
#define ABORTAR_ESI 117

// Saber si ocupa mas de una entrada el valor
#define	VARIAS_ENTRADAS true
#define	UNICA_ENTRADA false

#define SJF_SD 1001
#define SJF_CD 1002
#define HRRN 1003

/* For setsockopt(2) */
#define SOL_SOCKET	1

#define SO_DEBUG	1
#define SO_REUSEADDR	2
#define SO_TYPE		3
#define SO_ERROR	4

// Cantidad de clientes que puedo escuchar con el select
#define BACKLOG 20

int iniciarServidor(int);
void verificarErrorSocket(int);
void verificarErrorSetsockopt(int);
void verificarErrorBind(int, struct sockaddr_in);
void verificarErrorListen(int);
int aceptarConexionDeCliente(int);
int calcularSocketMaximo(int, int);
void sendRemasterizado(int, int, int, void*);
void sendDeNotificacion(int, uint32_t);
uint32_t recibirUint(int);
int recibirInt(int);
char* recibirString(int);
int recvDeNotificacion(int);
int conectarAServer(char*, int);
void realizarHandshake(int, uint32_t,uint32_t);



#endif /* SOCKET_H_ */
