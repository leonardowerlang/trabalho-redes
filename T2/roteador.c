#include "funcoes.h"

void *receber(void *arg);
void *processar(void *arg);
void *enviar(void *arg);
void *atualizar(void *arg);
void *timeout(void *arg);

pthread_mutex_t mt_bufferTimeout = PTHREAD_MUTEX_INITIALIZER, mt_bufferSaida = PTHREAD_MUTEX_INITIALIZER, mt_bufferEntrada = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mt_log = PTHREAD_MUTEX_INITIALIZER, mt_msgLog = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[]){
	LocalInfo info;
	int op = 1;
	Pacote *pacote = (Pacote *)malloc(sizeof(Pacote));

	if(argc != 2){
		printf("ERRO! Informe o arquivo executável e o número do roteador.\n");
		return 0;
	}

	inicializaRoteador(&info, char2int(argv[1]));

	pthread_create(&t_enviar, NULL, &enviar, &info);
	pthread_create(&t_receber, NULL, &receber, &info);
	pthread_create(&t_processar, NULL, &processar, &info);
	pthread_create(&t_atualizar, NULL, &atualizar, &info);
	pthread_create(&t_timeout, NULL, &timeout, &info);

	while(op){
		menu();
		scanf("%d", &op);

		switch(op){
			case 1:
				printf("Destino: ");
				scanf("%d", &pacote->idDestino);
				getchar();
				printf("MSG: ");
				scanf("%[^\n]s", pacote->msg);
				pacote = configurarPacote(0, 0, pacote->idDestino, info.id, pacote->msg);
				pushListaEspera(&info.bufferSaida, *pacote, 0, 0, &mt_bufferSaida);
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
				break;
			case 6:
				imprimirLista(info.bufferSaida);
				break;
			case 7:
				
				break;
			default:
				break;
		}
	}
	pthread_cancel(t_enviar);
	pthread_cancel(t_receber);
	pthread_cancel(t_processar);
	pthread_cancel(t_atualizar);
	pthread_cancel(t_timeout);
}


void *enviar(void *arg){
	LocalInfo *info = (LocalInfo*)arg;
	struct sockaddr_in socket_addr;
	int sckt, s_len = sizeof(socket_addr), tentativas;
	clock_t tempo;
	char log[50], aux[2];
	Roteador *r;
	Pacote pacote;
	aux[1] = '\0';

	while(1){
		if(info->bufferSaida == NULL){
			continue;
		}
		pacote = info->bufferSaida->pacote;
		r = getRoteador(info->roteadores, info->bufferSaida->pacote.idDestino);

		if(r == NULL){
			printf("ERRO! Roteador não existe!\n");
			popListaEspera(&info->bufferSaida, &mt_bufferSaida);
			continue;
		}

		inicializaSocket(&socket_addr, &sckt, r->porta);
		if(inet_aton(r->ip , &socket_addr.sin_addr) == 0){ // Verifica se o endereço de IP é valido
			printf("Endereço de IP invalido\n");
			exit(1);
		}

		if(pacote.tipo == 0){
			pacote.ack = info->ack;
			info->ack++;
		}

		strcpy(log, "Pacote de ");
		if(info->bufferSaida->pacote.tipo == 0){
			strcat(log, "dados");
		}else if(info->bufferSaida->pacote.tipo == 1){
			strcat(log, "confirmação");
		}else{
			strcat(log, "controle");
		}
		strcat(log, " enviado para [");
		aux[0] = (info->bufferSaida->pacote.idDestino) + '0';
		strcat(log, aux);
		strcat(log, "]");
		pushLog(&info->log, log, &mt_log);

		if(sendto(sckt, &pacote, sizeof(pacote) , 0 , (struct sockaddr *)&socket_addr, s_len) == -1){ // Envia a mensagem
			printf("Não foi possivel enviar a mensagem.\n");
			exit(1);
		}

		if(info->bufferSaida->pacote.tipo == 0 || info->bufferSaida->pacote.tipo == 3){
			tentativas = info->bufferSaida->tentativas;
			tempo = clock();
			pushListaEspera(&info->bufferTimeout, pacote, tentativas, tempo, &mt_bufferTimeout);
		}
		popListaEspera(&info->bufferSaida, &mt_bufferSaida);
	}	
}

void *atualizar(void *arg){
	LocalInfo *info = (LocalInfo*)arg;
	printf("%d\n", info->id);
	while(1);
}

void *processar(void *arg){
	LocalInfo *info = (LocalInfo*)arg;
	char log[50], aux[2];
	Pacote *pacote = (Pacote *)malloc(sizeof(Pacote));
	aux[1] = '\0';
	while(1){
		if(info->bufferEntrada == NULL){
			continue;
		}

		if(info->bufferEntrada->pacote.idDestino == info->id){
			if(info->bufferEntrada->pacote.tipo == 0){
				pushLog(&info->msg, info->bufferEntrada->pacote.msg, &mt_msgLog);

				strcpy(log, "Pacote de dados recebido de [");
				aux[0] = (info->bufferEntrada->pacote.idOrigem) + '0';
				strcat(log, aux);
				strcat(log, "]");
				pushLog(&info->log, log, &mt_log);

				pacote = configurarPacote(1, 0, info->bufferEntrada->pacote.idOrigem, info->id, "\0");
				pacote->ack = info->bufferEntrada->pacote.ack;
				pushListaEspera(&info->bufferSaida, *pacote, 0, 0, &mt_bufferSaida);
			}

			if(info->bufferEntrada->pacote.tipo == 1){

				strcpy(log, "Pacote de confirmação recebido de [");
				aux[0] = (info->bufferEntrada->pacote.idOrigem) + '0';
				strcat(log, aux);
				strcat(log, "]");
				pushLog(&info->log, log, &mt_log);
				removerListaEspera(&info->bufferTimeout, &info->bufferEntrada->pacote, &mt_bufferTimeout);
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
		pushListaEspera(&info->bufferEntrada, pacote, 0, 0, &mt_bufferEntrada);
	}
}

void *timeout(void * arg){
	LocalInfo *info = (LocalInfo*)arg;
	int tentativas;
	clock_t inicio, tempo;
	char log[50], aux[2];
	Pacote pacote;
	aux[1] = '\0';
	while(1){
		if(info->bufferTimeout == NULL){
			continue;
		}

		pthread_mutex_lock(&mt_bufferTimeout);
		inicio =  info->bufferTimeout->inicio;
		pacote = info->bufferTimeout->pacote;
		tentativas = info->bufferTimeout->tentativas;
		pthread_mutex_unlock(&mt_bufferTimeout);
		
		tempo = clock();
		if((double)(tempo - inicio)/CLOCKS_PER_SEC > TIMEOUT){

			strcpy(log, "Pacote enviado para [");
			aux[0] = (pacote.idDestino) + '0';
			strcat(log, aux);
			strcat(log, "] recebeu TIMEOUT");
			pushLog(&info->log, log, &mt_log);

			if(pacote.tipo == 0 && tentativas < 2){
				pushListaEspera(&info->bufferSaida, pacote, tentativas + 1, 0, &mt_bufferSaida);
			}
			popListaEspera(&info->bufferTimeout, &mt_bufferTimeout);
		}
	}
}