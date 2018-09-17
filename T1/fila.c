#include "fila.h"

void imprimir_fila(FPrioridade *fila){
	while(fila != NULL){
		printf("V:%d D:%d\n", fila->v, fila->distancia);
		fila = fila->prox;
	}
}

void push(FPrioridade **fila, int r, int distancia){		//Adiciona os elementos a fila
	FPrioridade *f = *fila, *novo, *aux;
	novo = (FPrioridade *)malloc(sizeof(FPrioridade));
	novo->v = r;
	novo->distancia = distancia;
	if(f == NULL){		//A fila esta vazia
		novo->prox = NULL;
		*fila = novo;
		return;
	}
	if(f->v == r){		//O vertice está na primeira posição da fila e atualiza o valor dele
		f->distancia = distancia;
		return;
	}
	if(f->distancia > distancia){		//O vertice possui menor distancia em relação ao primeiro elemento da fila
		novo->prox = f;
		*fila = novo;
		return;
	}
	aux = f;
	f = f->prox;
	while(f != NULL){
		if(f->v == r){		//O vertice ja existe na fila, somente att o valor
			f->distancia = distancia;
			return;
		}
		if(f->distancia > distancia){		//Add o vertice no meio da fila
			aux->prox = novo;
			novo->prox = f;
			return;
		}
		aux = f;
		f = f->prox;
	}
	aux->prox = novo;		//Add o vertice no final da fila
	novo->prox = NULL;
}

int get(FPrioridade *fila){		//Retorna o vertice do topo da fila
	if(fila == NULL){
		return -1;
	}
	return fila->v;
}

void pop(FPrioridade **fila){		//Remove o elemento do topo
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