#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>

#define MSG_SIZE 100
#define MAX_BUFFER 100
#define TIMEOUT 5
#define MAX_ROUT 100

pthread_t t_enviar, t_receber, t_processar, t_atualizar;

typedef struct{
	int tipo;		// 0 = dados e 1 = controle 2 = vetor de distancia
	int ack;		// Responsável por 
	int vetor_distancia[MAX_ROUT];
	int idDestino;	// Roteador Destino
	int idOrigem;	// Roteador Oerigem
	char msg[MSG_SIZE];	// Conteudo da mensagem
}Pacote;

typedef struct RT{
	int id;
	char ip[20];
	int porta;
	struct RT *prox;
}Roteador;

typedef struct TP{
	int id_0;
	int id_1;
	int distancia;
	int prox_salto;
	struct TP *prox;
}Topologia;

typedef struct LE{
	Pacote pacote;
	struct LE *prox;
}ListaEspera;

typedef struct BDLog{
	char msg[MSG_SIZE];
	struct BDLog *prox;
}Log;

typedef struct VZ{
	Roteador roteador;
	int prox_salto;
	int distancia;
	struct VZ *prox;
}Vizinhos;

typedef struct{
	int id;
	int porta;
	int ack;
	char ip[20];
	ListaEspera *bufferSaida, *bufferEntrada;
	Roteador *roteadores;
	Topologia *topologia;
	Log *msg;
	Log *log;
	int distancia[MAX_ROUT];
}LocalInfo;

#endif