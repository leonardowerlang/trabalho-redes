#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

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
	struct LE *prox;
}ListaEspera;