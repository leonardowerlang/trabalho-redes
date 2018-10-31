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
	int tipo;		// 1 se dados e 0 controle
	int idDestino;	// Roteador Destino
	int idOrigem;	// Roteador Oerigem
	char msg[MSG_SIZE];	// Conteudo da mensagem
}Pacote;

typedef struct{
	char ip[20];
	int porta;
}Roteador;

typedef struct LE{
	Pacote pacote;
	int i;
	struct LE *prox;
}ListaEspera;

typedef struct{
	ListaEspera *listaEspera, *listaProcessamento;
	int a, b;
}LocalInfo;