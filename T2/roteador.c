#include "config.h"

void *enviar(void *arg);
void *processar(void *arg);


int main(){
	printf("LUL\n");
	ListaEspera *listaEspera = NULL, *listaProcessamento = NULL;
	pthread_create(&t_enviar, NULL, &enviar, &listaEspera);
	pthread_create(&t_processar, NULL, &processar, &listaProcessamento);
	pthread_create(&t_receber, NULL, &receber, &listaProcessamento);
}

void *enviar(void *arg){
	ListaEspera *listaEspera = (ListaEspera*)arg;
	while(1){
		if(listaEspera == NULL){
			printf("enviar\n");
		}
	}
}

void *processar(void *arg){
	printf("processar\n");
}