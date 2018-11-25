#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "fila.h"

#define N_ROT 7			// Quantidade maxima de roteadores
#define MSG_SIZE 100 	// Tamanho maximo da msg

pthread_t t_receptor;

typedef struct{		// Estrutura da tabela de roteamento
	int v;			// Vértice predecessor
	int distancia;	// Distacia acumulada
}ii;

typedef struct{		// Estrutura do roteador
	int porta;
	char ip[20];
}Router;

typedef struct{		// Estrutura do pacote
	int idDestino, idOrigem;
	int p_destino;
	char ip_destino[20];
	char mensagem[MSG_SIZE];	
}TPacote;

typedef struct BD_Log{		// Estrutura dos LOGs
	char log[50];
	struct BD_Log *prox;
}BDLog;

typedef struct BD_Msg{		// Estrutura das mensagens
	char mensagem[MSG_SIZE];
	struct BD_Msg *prox;
}BDMsg;

typedef struct{		// Estrutura das informações locais
	int id, novas_msg;
	Router *roteadores;
	ii *tabela_roteamento;
	BDLog *log;
	BDMsg *msg;
}local_info;

void criar_grafo(int grafo[N_ROT][N_ROT]);
void imprimir_tabela(ii tabela[N_ROT]);
void dijkstra(int roteador, int grafo[N_ROT][N_ROT], ii tabela[N_ROT]);
void criar_tabela_roteamento(ii tabela[N_ROT], int roteador);

void add_log(BDLog **log, char msg[50]);
void imprimir_log(BDLog *log, int roteador);
void add_msg(BDMsg **mensagens, char msg[50]);
void imprimir_msg(BDMsg *msg, int roteador, int *novas_msg);
void imprimir_roteadores(Router roteadores[N_ROT]);
int char2int(char const *str);
void configura_roteadores(Router roteador[N_ROT]);
void inicializa_socket(struct sockaddr_in *socket_addr, int *sckt, int porta);
bool enviar_msg(ii tabela[], Router roteadores[], int id_roteador, TPacote pacote);
void enviar(ii tabela[N_ROT], Router roteadores[N_ROT], TPacote pacote, int id_roteador, BDLog **log);
void *receptor(void * arg);
void menu(int novas_msg, int roteador, char ip[20], int porta);
