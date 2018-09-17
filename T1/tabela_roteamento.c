#include "config.h"

void criar_grafo(int grafo[N_ROT][N_ROT]){		//Cria tabela de adjacencia do grafo
	FILE *arq = fopen("enlaces.config", "r");		//Abre o arquivo com o enlace da rede
	int id_1, id_0, distancia, i;
	if(arq == NULL){
		printf("Não foi possivel abrir o arquivo\n");
		fclose(arq);
		return;
	}
	while(fscanf(arq, "%d %d %d", &id_0, &id_1, &distancia) != EOF){		//Lê os dados do arquivo
		grafo[id_0][id_1] = grafo[id_1][id_0] = distancia;
	}
	fclose(arq);
	for (i = 0; i < N_ROT; i++){		//Zera a diagonal principal da matriz
		grafo[i][i] = 0;
	}
}

void imprimir_tabela(ii tabela[N_ROT]){
	system("clear");
	for(int i = 0; i < N_ROT; i++){
		printf("ID: %d\tDistancia: %d\tProximo: %d\n", i, tabela[i].distancia, tabela[i].v);
	}
	getchar();
	getchar();
}

void dijkstra(int roteador, int grafo[N_ROT][N_ROT], ii tabela[N_ROT]){
	int aberto[N_ROT], i, vertice;		//0 não foi passado pelo dijkstra
	FPrioridade *fila = NULL;

	memset(aberto, 0, sizeof(aberto));

	for (i = 0; i < N_ROT; i++){		//"Zera" a distancia da tabela
		tabela[i].v = -1;
		tabela[i].distancia = INT_MAX;
	}
	tabela[roteador].v = roteador;		//Att os dados do vertice inicial		
	tabela[roteador].distancia = 0;
	aberto[roteador] = 1;
	push(&fila, roteador, 0);		//Add o vertice inicial a lista de prioridade

	while(get(fila) >= 0){
		vertice = get(fila);		//Pega o vertice do topo da fila
		aberto[vertice] = 1;
		pop(&fila);		//Remove da fila o vertice
		for(i = 0; i < N_ROT; i++){		//Verifica os vertices adjacente e adciona eles a fila
			if((grafo[vertice][i] > 0) && (tabela[i].distancia > tabela[vertice].distancia + grafo[vertice][i]) && (aberto[i] == 0)){
				tabela[i].distancia = tabela[vertice].distancia + grafo[vertice][i];
				push(&fila, i, tabela[i].distancia);
				if(vertice == roteador){	//Encontra o proximo vertice do salto
					tabela[i].v = i;
				}else{
					tabela[i].v = tabela[vertice].v;
				}
			}
		}
	}
}

void criar_tabela_roteamento(ii tabela[N_ROT], int roteador){		//Cria a tabela de roteamento atraves do roteador inicial
	int grafo[N_ROT][N_ROT];
	memset(grafo, -1, sizeof(grafo));
	criar_grafo(grafo);
	dijkstra(roteador, grafo, tabela);
}

// int main(){
// 	ii tabela[N_ROT];
// 	int roteador = 0, i;
// 	criar_tabela_roteamento(tabela, roteador);
// 	printf("Vertice\tDistancia  Proximo\n");		//Printa tabela de roteamento
// 	for(i = 0; i < N_ROT; i++){
// 		printf("%d\t%d\t   %d\n", i, tabela[i].distancia, tabela[i].v);
// 	}
// }