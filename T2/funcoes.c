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

int char2int(char const *str){		// Converte char para int
	int a = 0, cont = 1, i;
	for (i = strlen(str) - 1; i >= 0; i--, cont *= 10){
		a += cont * (str[i] - '0');
	}
	return a;
}

void inicializaSocket(int *sockfd, struct sockaddr_in *serv_addr, int porta){
	if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\n Error : Could not create socket \n");
	}
	memset(serv_addr, '0', sizeof(struct sockaddr_in));
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_port = htons(porta);
	memset(serv_addr->sin_zero, 0x0, 8);
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
}

void inicializaRoteador(LocalInfo *info, int id){
	info->id = id;
	info->listaEspera = NULL;
	info->listaProcessamento = NULL;
	info->roteadores = NULL;

	lerRoteadores(info);
}