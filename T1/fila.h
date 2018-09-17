#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

typedef struct FilaPrioridade{		//Estrutura da fila de prioridade
	int v;		//Vertice
	int distancia;		//Distancia acumulada
	struct FilaPrioridade *prox;		//Proximo nodo da fila
}FPrioridade;

void imprimir_fila(FPrioridade *fila);
void push(FPrioridade **fila, int r, int distancia);
int get(FPrioridade *fila);
void pop(FPrioridade **fila);