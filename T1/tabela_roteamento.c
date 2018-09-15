#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

#define N_ROT 8		//Quantidade maxima de roteadores

typedef struct{		//Estrutura da tabela de roteamento
	int v;			//Vertice predecessor
	int distancia;	//Distacia acumulada
}ii;

typedef struct FilaPrioridade{		//Estrutura da fila de prioridade
	int v;							//Vertice
	int distancia;					//Distancia acumulada
	struct FilaPrioridade *prox;	//Proximo nodo da fila
}FPrioridade;

void criar_grafo(int grafo[N_ROT][N_ROT]){								//Cria tabela de adjacencia do grafo
	FILE *arq = fopen("enlaces.config", "r");							//Abre o arquivo com o enlace da rede
	int id_1, id_0, distancia, i;
	if(arq == NULL){
		printf("Não foi possivel abrir o arquivo\n");
		fclose(arq);
		return;
	}
	while(fscanf(arq, "%d %d %d", &id_0, &id_1, &distancia) != EOF){	//Lê os dados do arquivo
		grafo[id_0][id_1] = grafo[id_1][id_0] = distancia;
	}
	fclose(arq);
	for (i = 0; i < N_ROT; ++i){										//Zera a diagonal principal da matriz
		grafo[i][i] = 0;
	}
}

void imprimir_fila(FPrioridade *fila){
	while(fila != NULL){
		printf("V:%d D:%d\n", fila->v, fila->distancia);
		fila = fila->prox;
	}
}

void push(FPrioridade **fila, int r, int distancia){	//Adiciona os elementos a fila
	FPrioridade *f = *fila, *novo, *aux;
	novo = (FPrioridade *)malloc(sizeof(FPrioridade));
	novo->v = r;
	novo->distancia = distancia;
	if(f == NULL){										//A fila esta vazia
		novo->prox = NULL;
		*fila = novo;
		return;
	}
	if(f->v == r){										//O vertice está na primeira posição da fila e atualiza o valor dele
		f->distancia = distancia;
		return;
	}
	if(f->distancia > distancia){						//O vertice possui menor distancia em relação ao primeiro elemento da fila
		novo->prox = f;
		*fila = novo;
		return;
	}
	aux = f;
	f = f->prox;
	while(f != NULL){
		if(f->v == r){									//O vertice ja existe na fila, somente att o valor
			f->distancia = distancia;
			return;
		}
		if(f->distancia > distancia){					//Add o vertice no meio da fila
			aux->prox = novo;
			novo->prox = f;
			return;
		}
		aux = f;
		f = f->prox;
	}
	aux->prox = novo;									//Add o vertice no final da fila
	novo->prox = NULL;
}

int get(FPrioridade *fila){								//Retorna o vertice do topo da fila
	if(fila == NULL){
		return -1;
	}
	return fila->v;
}

void pop(FPrioridade **fila){							//Remove o elemento do topo
	FPrioridade *f = *fila, *aux = *fila;
	if(f->prox == NULL){
		*fila = NULL;
		free(f);
		return;
	}
	f = f->prox;
	*fila = f;
	free(aux);
}


void dijkstra(int roteador, int grafo[N_ROT][N_ROT]){
	ii tabela[N_ROT];						//Tabela de roteamento
	int aberto[N_ROT], i, vertice;			//1 não foi passado pelo dijkstra
	FPrioridade *fila = NULL;

	memset(aberto, 0, sizeof(aberto));

	for (i = 0; i < N_ROT; ++i){			//Zera a distancia da tabela
		tabela[i].v = -1;
		tabela[i].distancia = INT_MAX;
	}
	tabela[roteador].v = roteador;			//Att os dados do vertice inicial		
	tabela[roteador].distancia = 0;
	aberto[roteador] = 1;
	push(&fila, roteador, 0);				//Add o vertice inicial a lista de prioridade

	while(get(fila) >= 0){
		vertice = get(fila);				//Pega o vertice do topo da fila
		aberto[vertice] = 1;
		pop(&fila);							//Remove da fila o vertice
		for(i = 0; i < N_ROT; i++){			//Verifica os vertices adjacente e adciona eles a fila
			if((grafo[vertice][i] > 0) && (tabela[i].distancia > tabela[vertice].distancia + grafo[vertice][i]) && (aberto[i] == 0)){
				tabela[i].v = vertice;
				tabela[i].distancia = tabela[vertice].distancia + grafo[vertice][i];
				push(&fila, i, tabela[i].distancia);
			}
		}
	}
	printf("\nVertice\tDistancia  Proximo\n");	//Printa tabela
	for(i = 0; i < N_ROT; i++){
		printf("%d\t%d\t   %d\n", i, tabela[i].distancia, tabela[i].v);
	}


}

int main(){
	int grafo[N_ROT][N_ROT];
	memset(grafo, -1, sizeof(grafo));
	criar_grafo(grafo);
	for (int i = 0; i < N_ROT; i++){
		for (int j = 0; j < N_ROT; j++){
			printf("%d 	", grafo[i][j]);
		}
		printf("\n");
	}
	dijkstra(0, grafo);
}