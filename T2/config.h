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

#define MSG_SIZE 100

pthread_t t_enviar, t_receber, t_processar;

typedef struct{
	int tipo;		// 0 = dados e 1 = controle
	int ack;		// Responsável por 
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
	struct TP *prox;
}Topologia;

typedef struct LE{
	Pacote pacote;
	struct LE *prox;
}ListaEspera;

typedef struct{
	int id;
	int porta;
	char ip[20];
	ListaEspera *bufferSaida, *bufferEntrada;
	Roteador *roteadores;
	Topologia *topologia;
}LocalInfo;