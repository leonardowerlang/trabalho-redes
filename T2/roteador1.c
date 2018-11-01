#include "config.h"

void *enviar(void *arg);
void *processar(void *arg);
void *receber(void *arg);
void pushFila(ListaEspera **lista, Pacote pacote);
void imprimirFila(ListaEspera *lista);
void pop(ListaEspera **lista);

int char2int(char const *str){		// Converte char para int
	int a = 0, cont = 1, i;
	for (i = strlen(str) - 1; i >= 0; i--, cont *= 10){
		a += cont * (str[i] - '0');
	}
	return a;
}

void inicializa(char ip[20], int porta, )

void inicializaSocket(int *sockfd, struct sockaddr_in *serv_addr, int porta){
	if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		printf("\n Error : Could not create socket \n");
	}
	memset(serv_addr, '0', sizeof(struct sockaddr_in)); 
	serv_addr->sin_family = AF_INET;
	serv_addr->sin_port = htons(porta); 
}


int main(int argc, char const *argv[]){
	int op = 1;
	LocalInfo info;
	Pacote pacote;
	char msg[MSG_SIZE];


	if(argc != 2){
		printf("ERRO! Informe o arquivo executável e o número do roteador.\n");
		return 0;
	}

	if(char2int(argv[1]) == 0){
		info.a = 5000;
		info.b = 5001;
	}else{
		info.a = 5001;
		info.b = 5000;
	}

	printf("%d\n", info.a);
	info.listaEspera = NULL;
	info.listaProcessamento = NULL;
	pthread_create(&t_receber, NULL, &receber, &info);

	pthread_create(&t_enviar, NULL, &enviar, &info);
	pthread_create(&t_processar, NULL, &processar, &info);

	while(op){
		printf("Opção\n→ ");
		while(scanf("%d", &op) != 1){
			printf("Informe somente o número da opção desejada!\n→ ");
			getchar();
		}
		getchar();
		if(op == 1){
			printf("MSG: ");
			scanf("%[^\n]s", msg);
			strcpy(pacote.msg, msg);
			if(char2int(argv[1]) == 0){
				pacote.idDestino = 1;
				pacote.idOrigem = 0;	
			}else{
				pacote.idDestino = 0;
				pacote.idOrigem = 1;
			}
			pacote.tipo = 1;
			//printf("%p\n", info.listaEspera);
			pushFila(&info.listaEspera, pacote);
		}
	}
}

void *enviar(void *arg){
	LocalInfo *info = (LocalInfo*)arg;
	Pacote pacote;
	int sockfd = 0;
	struct sockaddr_in serv_addr; 

	inicializaSocket(&sockfd, &serv_addr, info->b);
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
		printf("\n inet_pton error occured\n");
	}


	while(1){
		if(info->listaEspera == NULL){
			sleep(1);
			continue;
		}
		pacote = info->listaEspera->pacote;

		if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){ // Tenta abrir conexão
			printf("\n Error : Connect Failed \n");
		}else{
			if(send(sockfd, &pacote, sizeof(pacote) , 0) == -1){ // Envia a mensagem
				printf("Não foi possivel enviar a mensagem.\n");
				exit(1);
			}
			pop(&info->listaEspera);
		}
	}


}

void *processar(void *arg){
	LocalInfo *info = (LocalInfo*)arg;
}

void *receber(void *arg){
	LocalInfo *info = (LocalInfo*)arg;

	Pacote pacote;

	pacote.idDestino = 20;

	/* Server socket structures */
	struct sockaddr_in serv_addr;

	/* File descriptors of connection and server */
	int serverfd = 0, connfd = 0;

	inicializaSocket(&serverfd, &serv_addr, info->a);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(serverfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	listen(serverfd, 1);

	fprintf(stdout, "Listening on port %d\n", info->a);


	while(1){
		connfd = accept(serverfd, (struct sockaddr*)NULL, NULL);

		if(recv(connfd, &pacote, sizeof(pacote), 0) == -1){
			printf("Deu merda\n");
		}
		printf("MENSAGEM: %s\n", pacote.msg);

	}
}

void pushFila(ListaEspera **lista, Pacote pacote){
	ListaEspera *l = *lista, *novo = (ListaEspera *)malloc(sizeof(ListaEspera));
	novo->prox = NULL;
	novo->i = 10;
	novo->pacote = pacote;
	if(*lista == NULL){
		*lista = novo;
		return;
	}
	*lista = novo;
}

void pop(ListaEspera **lista){
	*lista = NULL;
}

void imprimirFila(ListaEspera *lista){
	while(lista != NULL){
		printf("\nMENSAGEM: %s\n", lista->pacote.msg);
		lista = lista->prox;
	}
}