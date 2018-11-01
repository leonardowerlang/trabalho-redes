#include "funcoes.h"

void *receber(void *arg);
void *processar(void *arg);
void *enviar(void *arg);

int main(int argc, char const *argv[]){
	LocalInfo info;
	int op = 1;
	Pacote pacote;

	if(argc != 2){
		printf("ERRO! Informe o arquivo executável e o número do roteador.\n");
		return 0;
	}

	inicializaRoteador(&info, char2int(argv[1]));
	imprimirRoteadores(info.roteadores);
	imprimirTopologia(info.topologia);

	pthread_create(&t_receber, NULL, &receber, &info);
	pthread_create(&t_enviar, NULL, &enviar, &info);
	pthread_create(&t_processar, NULL, &processar, &info);

	while(op){
		printf("Opção → ");
		scanf("%d", &op);
		if(op == 1){
			printf("Destino: ");
			scanf("%d", &pacote.idDestino);
			getchar();
			printf("MSG: ");
			scanf("%[^\n]s", pacote.msg);
			pacote.tipo = 0;
			pacote.idOrigem = info.id;

			pushListaEspera(&info.bufferSaida, pacote);
		}
	}
}


void *enviar(void *arg){
	LocalInfo *info = (LocalInfo*)arg;
	struct sockaddr_in socket_addr;
	int sckt, s_len = sizeof(socket_addr);
	Roteador *r;
	Pacote pacote;

	while(1){
		if(info->bufferSaida == NULL){
			sleep(1);
			continue;
		}
		pacote = info->bufferSaida->pacote;
		r = getRoteador(info->roteadores, info->bufferSaida->pacote.idDestino);

		if(r == NULL){
			printf("ERRO! Roteador não existe!\n");
			popListaEspera(&info->bufferSaida);
			continue;
		}

		inicializaSocket(&socket_addr, &sckt, r->porta);
		if(inet_aton(r->ip , &socket_addr.sin_addr) == 0){ // Verifica se o endereço de IP é valido
			printf("Endereço de IP invalido\n");
			exit(1);
		}

		if(sendto(sckt, &pacote, sizeof(pacote) , 0 , (struct sockaddr *)&socket_addr, s_len) == -1){ // Envia a mensagem
			printf("Não foi possivel enviar a mensagem.\n");
			exit(1);
		}

		popListaEspera(&info->bufferSaida);
	}	
}

void *processar(void *arg){
	LocalInfo *info = (LocalInfo*)arg;
	while(1){
		if(info->bufferEntrada == NULL){
			sleep(1);
			continue;
		}

		if(info->bufferEntrada->pacote.idDestino == info->id){
			printf("Tipo: %d\tDest: %d\tOrig: %d\n", info->bufferEntrada->pacote.tipo,
			 										info->bufferEntrada->pacote.idDestino,
			 										info->bufferEntrada->pacote.idOrigem);
			printf("MSG: %s\n", info->bufferEntrada->pacote.msg);
		}
		popListaEspera(&info->bufferEntrada);
	}
}

void *receber(void *arg){
	LocalInfo *info = (LocalInfo*)arg;
	struct sockaddr_in socket_addr;
	int  sckt, s_len = sizeof(socket_addr);
	Pacote pacote;

	inicializaSocket(&socket_addr, &sckt, info->porta);

	if(bind(sckt, (struct sockaddr *) &socket_addr, s_len) == -1){	// Liga um nome ao socket
		printf("A ligacao do socket com a porta falhou.\n");
		exit(1);
	}

	while(1){
		if(recvfrom(sckt, &pacote, sizeof(pacote), 0, (struct sockaddr *)&socket_addr, (uint *)&s_len) == -1){// Recebe as mensagens do socket
			printf("ERRO ao receber os pacotes.\n");
		}
		pushListaEspera(&info->bufferEntrada, pacote);
	}
}