#include "config.h"

void criar_grafo(int grafo[N_ROT][N_ROT]){		// Cria tabela de adjacência do grafo
	FILE *arq = fopen("enlaces.config", "r");		// Abre o arquivo com o enlace da rede
	int id_1, id_0, distancia, i;
	if(arq == NULL){
		printf("Não foi possivel abrir o arquivo\n");
		fclose(arq);
		return;
	}
	while(fscanf(arq, "%d %d %d", &id_0, &id_1, &distancia) != EOF){		// Lê os dados do arquivo
		grafo[id_0][id_1] = grafo[id_1][id_0] = distancia;
	}
	fclose(arq);
	for (i = 0; i < N_ROT; i++){		// Zera a diagonal principal da matriz
		grafo[i][i] = 0;
	}
}

void imprimir_tabela(ii tabela[N_ROT]){		// Mostra a tabela de roteamento
	system("clear");
	printf(" _______________________________________________________________________ \n");
	printf("|\tID\t|\tDistância\t|\tPróximo Salto\t\t|\n");
	printf("|---------------|-----------------------|-------------------------------|\n");
	for(int i = 0; i < N_ROT; i++){
		printf("|\t%d\t|\t    %d\t\t|\t\t%d\t\t|\n", i, tabela[i].distancia, tabela[i].v);
	}
	printf("|_______________________________________________________________________|\n\n");
	printf(" Pressione ENTER para continuar... ");
	getchar();
	getchar();
}

void dijkstra(int roteador, int grafo[N_ROT][N_ROT], ii tabela[N_ROT]){
	int aberto[N_ROT], i, vertice;		// Aberto = 0 não foi passado pelo dijkstra
	FPrioridade *fila = NULL;

	memset(aberto, 0, sizeof(aberto));

	for (i = 0; i < N_ROT; i++){		// "Zera" a distância da tabela
		tabela[i].v = -1;
		tabela[i].distancia = INT_MAX;
	}
	tabela[roteador].v = roteador;		// Att os dados do vértice inicial		
	tabela[roteador].distancia = 0;
	aberto[roteador] = 1;
	push(&fila, roteador, 0);		// Add o vértice inicial a fila de prioridade

	while(get(fila) >= 0){
		vertice = get(fila);		// Pega o vértice do topo da fila
		aberto[vertice] = 1;
		pop(&fila);					// Remove da fila o vértice
		for(i = 0; i < N_ROT; i++){		//Verifica os vértices adjacente e adciona eles a fila
			if((grafo[vertice][i] > 0) && (tabela[i].distancia > tabela[vertice].distancia + grafo[vertice][i]) && (aberto[i] == 0)){
				tabela[i].distancia = tabela[vertice].distancia + grafo[vertice][i];
				push(&fila, i, tabela[i].distancia);
				if(vertice == roteador){	// Encontra o próximo vértice do salto
					tabela[i].v = i;		// Vizinho do vértice inicial
				}else{
					tabela[i].v = tabela[vertice].v;	// Não é vizinho do vérice inicial
				}
			}
		}
	}
}

void criar_tabela_roteamento(ii tabela[N_ROT], int roteador){		// Cria a tabela de roteamento a partir do roteador inicial
	int grafo[N_ROT][N_ROT];
	memset(grafo, -1, sizeof(grafo));
	criar_grafo(grafo);
	dijkstra(roteador, grafo, tabela);
}