#include "funcoes.h"

void *receber(void *arg);
void *processar(void *arg);
void *enviar(void *arg);

pthread_mutex_t mt_enviar = PTHREAD_MUTEX_INITIALIZER, mt_bufferSaida = PTHREAD_MUTEX_INITIALIZER, mt_bufferEntrada = PTHREAD_MUTEX_INITIALIZER;

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

	pthread_create(&t_enviar, NULL, &enviar, &info);
	pthread_create(&t_receber, NULL, &receber, &info);
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
			pushListaEspera(&info.bufferSaida, pacote, &mt_bufferSaida);
		}
	}
	pthread_cancel(t_enviar);
	pthread_cancel(t_receber);
	pthread_cancel(t_processar);

}


void *enviar(void *arg){
	LocalInfo *info = (LocalInfo*)arg;
	struct sockaddr_in socket_addr;
	int sckt, s_len = sizeof(socket_addr);
	Roteador *r;
	Pacote pacote;
	time_t inicio, fim;

	while(1){
		if(info->bufferSaida == NULL){
			continue;
		}
		if(pthread_mutex_trylock(&mt_enviar) == 0){
			pacote = info->bufferSaida->pacote;
			r = getRoteador(info->roteadores, info->bufferSaida->pacote.idDestino);

			if(r == NULL){
				printf("ERRO! Roteador não existe!\n");
				popListaEspera(&info->bufferSaida, &mt_bufferSaida);
				continue;
			}
			if(pacote.tipo == 0){
				pacote.ack = info->ack;
				info->ack++;
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
			if(pacote.tipo == 0){
				inicio = time(NULL);
				fim = inicio;
				while(pthread_mutex_trylock(&mt_enviar) != 0){
					if(fim - inicio >= TIMEOUT){
						printf("TIMEOUT!\n");
						break;
					}
					fim = time(NULL);
				}
			}
			pthread_mutex_unlock(&mt_enviar);
			popListaEspera(&info->bufferSaida, &mt_bufferSaida);
		}
	}	
}

void *processar(void *arg){
	LocalInfo *info = (LocalInfo*)arg;
	Pacote pacote;
	while(1){
		if(info->bufferEntrada == NULL){
			continue;
		}

		if(info->bufferEntrada->pacote.idDestino == info->id){
			imprimirPacote(&info->bufferEntrada->pacote);
		}

		if(info->bufferEntrada->pacote.tipo == 1 && info->bufferEntrada->pacote.ack == info->ack - 1){
			printf("Pacote confirmado!\n");
			pthread_mutex_unlock(&mt_enviar);
		}

		if(info->bufferEntrada->pacote.tipo == 0 && info->bufferEntrada->pacote.idDestino == info->id){
			if(info->bufferEntrada->pacote.idDestino == info->bufferEntrada->pacote.idOrigem){ // Pacote para o proprio roteador
				pthread_mutex_unlock(&mt_enviar);
			}else{
				pacote.idOrigem = info->id;
				pacote.idDestino = info->bufferEntrada->pacote.idOrigem;
				pacote.tipo = 1;
				pacote.ack = info->bufferEntrada->pacote.ack;
				pacote.msg[0] = '\0';
				pushListaEspera(&info->bufferSaida, pacote, &mt_bufferSaida);
			}
		}
		popListaEspera(&info->bufferEntrada, &mt_bufferEntrada);
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
		pushListaEspera(&info->bufferEntrada, pacote, &mt_bufferEntrada);
	}
}