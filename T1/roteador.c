#include "config.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char const *argv[]){
	int id_roteador, op = 1;
	Router roteadores[N_ROT];
	ii tabela[N_ROT];
	local_info info;
	TPacote pacote;

	if(argc != 2){
		printf("ERRO! Informe o arquivo executável e o número do roteador.\n");
		return 0;
	}
	id_roteador = char2int(argv[1]);
	printf("%d\n", id_roteador);
	criar_tabela_roteamento(tabela, id_roteador);	// Cria a tabela de roteamento
	configura_roteadores(roteadores);				// Lê o IP e a porta de cada roteador

	info.id = id_roteador;			// Define os valores das variáveis locais
	info.novas_msg = 0;
	info.tabela_roteamento = tabela;
	info.roteadores = roteadores;
	info.log = NULL;
	info.msg = NULL;

	pthread_create(&t_receptor, NULL, &receptor, &info);	//Thread responsável por receber as mensagens

	while(op){
		menu(info.novas_msg, id_roteador, roteadores[id_roteador].ip, roteadores[id_roteador].porta);
		while(scanf("%d", &op) != 1){
			printf("Informe somente o número da opção desejada!\n→ ");
			getchar();
		}
		switch(op){
			case 1:
				if(pthread_mutex_lock(&mutex) == 0){
					imprimir_tabela(info.tabela_roteamento);	// Mostra a tabela de roteamento
					pthread_mutex_unlock(&mutex);
				}
				break;
			case 2:
				if(pthread_mutex_lock(&mutex) == 0){
					imprimir_roteadores(info.roteadores);		// Mostra as informações dos roteadores
					pthread_mutex_unlock(&mutex);
				}
				break;
			case 3:
				if(pthread_mutex_lock(&mutex) == 0){
					enviar(tabela, roteadores, pacote, id_roteador, &info.log);	// Envia uma mensagem
					sleep(1);
					pthread_mutex_unlock(&mutex);
				}
				break;
			case 4:
				if(pthread_mutex_lock(&mutex) == 0){
					imprimir_msg(info.msg, id_roteador, &info.novas_msg);	// Mostra as mensagens recebidas
					pthread_mutex_unlock(&mutex);
				}
				break;
			case 5:
				if(pthread_mutex_lock(&mutex) == 0){
					imprimir_log(info.log, id_roteador);		// Mostra os LOGs do roteador
					pthread_mutex_unlock(&mutex);
				}
				break;
			default:
				break;
		}
	}
	return 0;
}

void add_log(BDLog **log, char msg[50]){		// Adiciona um LOG a lista
	BDLog *l = *log, *novo;
	novo = (BDLog *)malloc(sizeof(BDLog));
	strcpy(novo->log, msg);
	novo->prox = l;
	*log = novo;
}

void imprimir_log(BDLog *log, int roteador){	// Imprime todos os LOGs da lista, último LOG no topo
	system("clear");
	printf(" _______________________________________________________________________ \n");
	printf("|\t\t\tLOG do Roteador %d\t\t\t\t|\n", roteador);
	printf("|_______________________________________________________________________|\n\n");

	if(log == NULL){
		printf(" Nenhum LOG encontrado!\n");
	}
	while(log != NULL){
		printf(" -> %s\n", log->log);
		log = log->prox;
	}
	printf("\n Pressione ENTER para continuar... ");
	getchar();
	getchar();
}

void add_msg(BDMsg **mensagens, char msg[50]){	// Adiciona uma mensagem a lista
	BDMsg *m = *mensagens, *novo;
	novo = (BDMsg *)malloc(sizeof(BDMsg));
	strcpy(novo->mensagem, msg);
	novo->prox = m;
	*mensagens = novo;
}

void imprimir_msg(BDMsg *msg, int roteador, int *novas_msg){	// Mostra todas as mensagens da lista, última msg no topo
	*novas_msg = 0;
	system("clear");
	printf(" _______________________________________________________________________ \n");
	printf("|\t\t\tMenssagens do Roteador %d\t\t\t|\n", roteador);
	printf("|_______________________________________________________________________|\n\n");
	if(msg == NULL){
		printf(" Nenhuma mensagem encontrada!\n");
	}
	while(msg != NULL){
		printf(" -> %s\n", msg->mensagem);
		msg = msg->prox;
	}
	printf("\n Pressione ENTER para continuar... ");
	getchar();
	getchar();
}


void imprimir_roteadores(Router roteadores[N_ROT]){		// Mostra o IP e porta de todos os roteadores
	system("clear");
	printf(" _______________________________________________________________________ \n");
	printf("|\tID\t|\t\tIP\t\t|\tPorta\t\t|\n");
	printf("|---------------|-------------------------------|-----------------------|\n");

	for (int i = 0; i < N_ROT; i++){
		printf("|\t%d\t|\t%s\t\t|\t%d\t\t|\n", i, roteadores[i].ip, roteadores[i].porta);
	}
	printf("|_______________________________________________________________________|\n\n");
	printf(" Pressione ENTER para continuar... ");
	getchar();
	getchar();
}

int char2int(char const *str){		// Converte char para int
	int a = 0, cont = 1, i;
	for (i = strlen(str) - 1; i >= 0; i--, cont *= 10){
		a += cont * (str[i] - '0');
	}
	return a;
}

void configura_roteadores(Router roteador[N_ROT]){		// Lê o IP e porta dos roteadores
	FILE *arq = fopen("roteador.config", "r");
	int id, porta;
	char ip[20];

	if(arq == NULL){
		printf("Não foi possivel abrir o arquivo.\n");
		fclose(arq);
		return;
	}
	while(fscanf(arq, "%d %d %s", &id, &porta, ip) != EOF){		//Lê os dados do arquivo
		roteador[id].porta = porta;
		strcpy(roteador[id].ip, ip);
	}
	fclose(arq);
}

void inicializa_socket(struct sockaddr_in *socket_addr, int *sckt, int porta){	// Inicializa o socket
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

bool enviar_msg(ii tabela[], Router roteadores[], int id_roteador, TPacote pacote){	// Envia os pacotes para o roteador destino
	struct sockaddr_in socket_addr;
	int sckt, s_len = sizeof(socket_addr), proximo;

	proximo = tabela[pacote.idDestino].v;

	inicializa_socket(&socket_addr, &sckt, roteadores[proximo].porta);

	if(inet_aton(roteadores[proximo].ip , &socket_addr.sin_addr) == 0){ // Verifica se o endereço de IP é valido
		printf("Endereço de IP invalido\n");
		exit(1);
		return false;
	}

	if(sendto(sckt, &pacote, sizeof(pacote) , 0 , (struct sockaddr *)&socket_addr, s_len) == -1){ // Envia a mensagem
		printf("Não foi possivel enviar a mensagem.\n");
		exit(1);
		return false;
	}
	return true;
}

void enviar(ii tabela[N_ROT], Router roteadores[N_ROT], TPacote pacote, int id_roteador, BDLog **log){	// Lê a mensagem e cria o pacote para enviar
	int r_destino;
	char msg[500], msg_log[MSG_SIZE], aux[2];
	aux[1] = '\0';

	system("clear");
	imprimir_roteadores(roteadores);
	printf(" Informe o ID do roteador destino → ");
	while(scanf("%d", &r_destino) != 1 || r_destino >= N_ROT){		// Lê o ID do roteador destino
		printf(" Informe o ID do roteador destino → ");
		getchar();
	}
	printf(" Digite a mensagem → ");
	getchar();
	scanf("%[^\n]s", msg);
	while(strlen(msg) > MSG_SIZE){
		printf(" O tamnaho máximo da mensagem é de 100 caracteres.\n");		// Lê a mensagem
		printf(" Digite a mensagem → ");
		getchar();
		scanf("%[^\n]s", msg);
	}

	pacote.idDestino = r_destino;		// Cria o pacote
	pacote.idOrigem = id_roteador;
	strcpy(pacote.ip_destino, roteadores[r_destino].ip);
	pacote.p_destino = roteadores[r_destino].porta;
	strcpy(pacote.mensagem, msg);

	strcpy(msg_log, "Pacote enviado para [");	// Cria um LOG
	aux[0] = (pacote.idDestino) + '0';
	strcat(msg_log, aux);
	strcat(msg_log, "]!");

	if(enviar_msg(tabela, roteadores, id_roteador, pacote)){	// Manda a mensagem e se não houver falha adiciona o LOG
		add_log(log, msg_log);
	}
}

void *receptor(void * arg){		// Recebe os pacotes
	local_info *info = (local_info*)arg;
	struct sockaddr_in socket_addr;
	int id_roteador = info->id, sckt, s_len = sizeof(socket_addr);
	char log[MSG_SIZE], aux[2];
	aux[1] = '\0';
	Router *roteadores = info->roteadores;
	ii *tabela = info->tabela_roteamento;
	TPacote pacote;

	inicializa_socket(&socket_addr, &sckt, roteadores[id_roteador].porta);

	if(bind(sckt, (struct sockaddr *) &socket_addr, s_len) == -1){	// Liga um nome ao socket
		printf("A ligacao do socket com a porta falhou.\n");
		exit(1);
	}

	while(true){
		if(recvfrom(sckt, &pacote, sizeof(pacote), 0, (struct sockaddr *)&socket_addr, (uint *)&s_len) == -1){// Recebe as mensagens do socket
			printf("ERRO ao receber os pacotes.\n");
		}

		if(strcmp(pacote.ip_destino, roteadores[id_roteador].ip) == 0 && pacote.p_destino == roteadores[id_roteador].porta){	// Verifica se o pacote era para o roteador atual, se não for encaminha para o próximo roteador
			strcpy(log, "Pacote recebido de [");
			aux[0] = (pacote.idOrigem) + '0';
			strcat(log, aux);
			strcat(log, "]!");
			add_log(&info->log ,log);
			add_msg(&info->msg, pacote.mensagem);	
			info->novas_msg++;
			if(pthread_mutex_trylock(&mutex) == 0){		// Atualiza o número de mensagens recebidas no menu
				menu(info->novas_msg, id_roteador, roteadores[id_roteador].ip, roteadores[id_roteador].porta);
				printf("\n");
				pthread_mutex_unlock(&mutex);
			}
		}else{	// Encamhina para o próximo roteador
			strcpy(log, "Pacote de [");
			aux[0] = (pacote.idOrigem) + '0';
			strcat(log, aux);
			strcat(log, "] encaminhado para [");
			aux[0] = (pacote.idDestino) + '0';
			strcat(log, aux);
			strcat(log, "]!");
			if(enviar_msg(tabela, roteadores, id_roteador, pacote)){
				add_log(&info->log ,log);
			}
		}
	}
}

void menu(int novas_msg, int roteador, char ip[20], int porta){
	system("clear");
	printf(" _______________________________________________________________________ \n");
	printf("| Roteador: %d\t\tIP: %s\t\tPorta: %d\t\t|\n", roteador, ip, porta);
	printf("|-----------------------------------------------------------------------|\n");
	printf("| Você tem %d novas menssagens!\t\t\t\t\t\t|\n", novas_msg);
	printf("|_______________________________________________________________________|\n");
	printf("| \tNº da Opção\t\t| \t\tOpção\t\t\t|\n");
	printf("|-------------------------------|---------------------------------------|\n");
	printf("|→ 0\t\t\t\t| Sair\t\t\t\t\t|\n");
	printf("|→ 1\t\t\t\t| Mostrar Tabela de Roteamento\t\t|\n");
	printf("|→ 2\t\t\t\t| Mostrar Informações dos Roteadores\t|\n");
	printf("|→ 3\t\t\t\t| Enviar Mensagem\t\t\t|\n");
	printf("|→ 4\t\t\t\t| Mostrar Mensagens Recebidas\t\t|\n");
	printf("|→ 5\t\t\t\t| Mostrar LOG\t\t\t\t|\n");
	printf("|_______________________________________________________________________|\n");
	printf("→ ");
}

