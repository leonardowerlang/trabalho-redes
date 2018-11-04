#include "funcoes.h"

void *receber(void *arg);
void *processar(void *arg);
void *enviar(void *arg);

pthread_mutex_t mt_enviar = PTHREAD_MUTEX_INITIALIZER, mt_bufferSaida = PTHREAD_MUTEX_INITIALIZER, mt_bufferEntrada = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mt_log = PTHREAD_MUTEX_INITIALIZER, mt_msgLog = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[]){
	LocalInfo info;
	int op = 1;
	Pacote pacote;

	if(argc != 2){
		printf("ERRO! Informe o arquivo executável e o número do roteador.\n");
		return 0;
	}

	inicializaRoteador(&info, char2int(argv[1]));

	pthread_create(&t_enviar, NULL, &enviar, &info);
	pthread_create(&t_receber, NULL, &receber, &info);
	pthread_create(&t_processar, NULL, &processar, &info);

	while(op){
		menu();
		scanf("%d", &op);

		switch(op){
			case 1:
				printf("Destino: ");
				scanf("%d", &pacote.idDestino);
				getchar();
				printf("MSG: ");
				scanf("%[^\n]s", pacote.msg);
				pacote.tipo = 0;
				pacote.idOrigem = info.id;
				pushListaEspera(&info.bufferSaida, pacote, &mt_bufferSaida);
				break;
			case 2:
				imprimirRoteadores(info.roteadores);
				break;
			case 3:
				imprimirTopologia(info.topologia);
				break;
			case 4:
				imprimirMSG(info.msg);
				break;
			case 5:
				imprimirMSG(info.log);
			default:
				break;
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
	char log[50], aux[2];
	Roteador *r;
	Pacote pacote;
	time_t inicio, fim;
	aux[1] = '\0';

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

			strcpy(log, "Pacote de ");
			info->bufferSaida->pacote.tipo ? strcat(log, "controle") : strcat(log, "dados");
			strcat(log, " enviado para [");
			aux[0] = (info->bufferSaida->pacote.idDestino) + '0';
			strcat(log, aux);
			strcat(log, "]\n");
			pushLog(&info->log, log, &mt_log);

			if(pacote.tipo == 0){
				inicio = time(NULL);
				fim = inicio;
				while(pthread_mutex_trylock(&mt_enviar) != 0){
					if(fim - inicio >= TIMEOUT){

						strcpy(log, "Pacote enviado para [");
						aux[0] = (info->bufferSaida->pacote.idDestino) + '0';
						strcat(log, aux);
						strcat(log, "] recebeu TIMEOUT!\n");
						pushLog(&info->log, log, &mt_log);

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
	char log[50], aux[2];
	aux[1] = '\0';
	Pacote pacote;
	while(1){
		if(info->bufferEntrada == NULL){
			continue;
		}

		if(info->bufferEntrada->pacote.idDestino == info->id){
			if(info->bufferEntrada->pacote.tipo == 1 && info->bufferEntrada->pacote.ack == info->ack - 1){

				strcpy(log, "Pacote confirmado pelo roteador [");
				aux[0] = (info->bufferEntrada->pacote.idOrigem) + '0';
				strcat(log, aux);
				strcat(log, "]\n");
				pushLog(&info->log, log, &mt_log);

				pthread_mutex_unlock(&mt_enviar);
			}

			if(info->bufferEntrada->pacote.tipo == 0){
				pushLog(&info->msg, info->bufferEntrada->pacote.msg, &mt_msgLog);
				if(info->bufferEntrada->pacote.idDestino == info->bufferEntrada->pacote.idOrigem){ // Pacote para o proprio roteador

					strcpy(log, "Pacote recebido de [");
					aux[0] = (info->bufferEntrada->pacote.idOrigem) + '0';
					strcat(log, aux);
					strcat(log, "]\n");
					pushLog(&info->log, log, &mt_log);

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