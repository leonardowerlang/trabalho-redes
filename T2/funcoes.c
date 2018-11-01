#include "funcoes.h"

Roteador *getRoteador(Roteador *r, int id){
	while(r != NULL){
		if(r->id == id){
			return r;
		}
		r = r->prox;
	}
	return NULL;
}

void pushRoteadores(Roteador **roteadores, int id, int porta, char ip[20]){
	Roteador *r = *roteadores, *novo = (Roteador *)malloc(sizeof(Roteador));
	novo->id = id;
	novo->porta = porta;
	strcpy(novo->ip, ip);
	novo->prox = r;
	*roteadores = novo;
}

void imprimirRoteadores(Roteador *r){
	while(r != NULL){
		printf("ID: %d\tPorta: %d\tIP: %s\n", r->id, r->porta, r->ip);
		r = r->prox;
	}
}

void pushTopologia(Topologia **topologia, int id_0, int id_1, int distancia){
	Topologia *t = *topologia, *novo = (Topologia *)malloc(sizeof(Topologia));
	novo->id_0 = id_0;
	novo->id_1 = id_1;
	novo->distancia = distancia;
	novo->prox = t;
	*topologia = novo;
}

void imprimirTopologia(Topologia *t){
	while(t != NULL){
		printf("ID0: %d\tID1: %d\tDistancia: %d\n", t->id_0, t->id_1, t->distancia);
		t = t->prox;
	}
}

void pushListaEspera(ListaEspera **lista, Pacote pacote){
	ListaEspera *l = *lista, *novo = (ListaEspera *)malloc(sizeof(ListaEspera));
	novo->pacote = pacote;
	novo->prox = l;
	*lista = novo;
}

void popListaEspera(ListaEspera **lista){
	ListaEspera *l = *lista;
	*lista = l->prox;
	free(l);
}

void imprimirLista(ListaEspera *lista){
	while(lista != NULL){
		printf("MSG: %s\n", lista->pacote.msg);
		lista = lista->prox;
	}
}

int char2int(char const *str){		// Converte char para int
	int a = 0, cont = 1, i;
	for (i = strlen(str) - 1; i >= 0; i--, cont *= 10){
		a += cont * (str[i] - '0');
	}
	return a;
}

void inicializaSocket(struct sockaddr_in *socket_addr, int *sckt, int porta){	// Inicializa o socket
	int s_len = sizeof(socket_addr);
	if((*sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		printf("Falha ao criar Socket.\n");
		exit(1);
	}else{
		memset((char *) socket_addr, 0, s_len);
		socket_addr->sin_family = AF_INET;
		socket_addr->sin_port = htons(porta); 
		socket_addr->sin_addr.s_addr = htonl(INADDR_ANY); 
	}
}

void lerRoteadores(LocalInfo *info){
	FILE *arq = fopen("roteador.config", "r");		// Abre o arquivo com o enlace da rede
	int porta, id;
	char ip[20];
	if(arq == NULL){
		printf("Não foi possivel abrir o arquivo\n");
		fclose(arq);
		return;
	}
	while(fscanf(arq, "%d %d %s", &id, &porta, ip) != EOF){		// Lê os dados do arquivo
		pushRoteadores(&info->roteadores, id, porta, ip);
		if(info->id == id){
			info->porta = porta;
			strcpy(info->ip, ip);
		}
	}
	fclose(arq);
}

void lerTopologia(LocalInfo *info){
	FILE *arq = fopen("enlaces.config", "r");		// Abre o arquivo com o enlace da rede
	int id_0, id_1, distancia;
	if(arq == NULL){
		printf("Não foi possivel abrir o arquivo\n");
		fclose(arq);
		return;
	}
	while(fscanf(arq, "%d %d %d", &id_0, &id_1, &distancia) != EOF){		// Lê os dados do arquivo
		pushTopologia(&info->topologia, id_0, id_1, distancia);
	}
	fclose(arq);
}

void inicializaRoteador(LocalInfo *info, int id){
	printf("ID: %d\n", id);
	info->id = id;
	info->bufferSaida = NULL;
	info->bufferEntrada = NULL;
	info->roteadores = NULL;
	info->topologia = NULL;

	lerRoteadores(info);
	lerTopologia(info);
}