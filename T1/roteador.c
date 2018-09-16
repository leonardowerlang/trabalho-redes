#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include "tabela_roteamento.c"

pthread_t t_receptor;

typedef struct{
	int porta;
	char ip[20];
}Router;

typedef struct{
	int origem, destino;
	int p_origem, p_destino;
	char mensagem[MSG_SIZE];	
}TPacote;

typedef struct BD_Log{
	char log[50];
	struct BD_Log *prox;
}BDLog;

typedef struct BD_Msg{
	char mensagem[MSG_SIZE];
	struct BD_Msg *prox;
}BDMsg;

typedef struct{
	int id;
	Router *roteadores;
	ii *tabela_roteamento;
	BDLog *log;
	BDMsg *msg;
}local_info;


void add_log(BDLog **log, char msg[50]){
	BDLog *l = *log, *novo;
	novo = (BDLog *)malloc(sizeof(BDLog));
	strcpy(novo->log, msg);
	novo->prox = l;
	*log = novo;
}

void imprimir_log(BDLog *log){
	printf("LOG\n");
	while(log != NULL){
		printf("-> %s\n", log->log);
		log = log->prox;
	}
	getchar();
	getchar();
}

void add_msg(BDMsg **mensagens, char msg[50]){
	BDMsg *m = *mensagens, *novo;
	novo = (BDMsg *)malloc(sizeof(BDMsg));
	strcpy(novo->mensagem, msg);
	novo->prox = m;
	*mensagens = novo;
}

void imprimir_msg(BDMsg *msg){
	printf("MESSAGENS\n");
	if(msg == NULL){
		printf("Nenhuma mensagem encontrada!\n");
	}
	while(msg != NULL){
		printf("%s\n", msg->mensagem);
		msg = msg->prox;
	}
	getchar();
	getchar();
}


void imprimir_roteadores(Router roteadores[N_ROT]){
	system("clear");
	for (int i = 0; i < N_ROT; i++){
		printf("ID: %d\tPorta: %d\tIP%s\n", i, roteadores[i].porta, roteadores[i].ip);
	}
	getchar();
	getchar();
}

int char2int(char const *str){
	int a = 0, cont = 1, i;
	for (i = strlen(str) - 1; i >= 0; i--, cont *= 10){
		a += cont * (str[i] - '0');
	}
	return a;
}

void configura_roteadores(Router roteador[N_ROT]){
	FILE *arq = fopen("roteador.config", "r");
	int id, porta;
	char ip[20];

	if(arq == NULL){
		printf("Não foi possivel abrir o arquivo\n");
		fclose(arq);
		return;
	}
	while(fscanf(arq, "%d %d %s", &id, &porta, ip) != EOF){		//Lê os dados do arquivo
		roteador[id].porta = porta;
		strcpy(roteador[id].ip, ip);
	}
	fclose(arq);
}

void inicializa_socket(struct sockaddr_in *socket_addr, int *sckt, int porta){
	int s_len = sizeof(socket_addr);
	if((*sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		printf("Falha ao criar Socket!");
		exit(1);
	}else{
		memset((char *) socket_addr, 0, s_len);
		socket_addr->sin_family = AF_INET;
		socket_addr->sin_port = htons(porta); //Porta em ordem de bytes de rede
		socket_addr->sin_addr.s_addr = htonl(INADDR_ANY); //Atribui o socket a todo tipo de interface
	}
}

void enviar_msg(ii tabela[], Router roteadores[], int id_roteador, TPacote pacote){
	struct sockaddr_in socket_addr;
	int sckt, s_len = sizeof(socket_addr), proximo;

	proximo = tabela[pacote.destino].v;

	inicializa_socket(&socket_addr, &sckt, roteadores[proximo].porta);

	if(inet_aton(roteadores[proximo].ip , &socket_addr.sin_addr) == 0){
		printf("inet_atonfailed\n");
		exit(1);
	}

	if(sendto(sckt, &pacote, sizeof(pacote) , 0 , (struct sockaddr *)&socket_addr, s_len)==-1){
		printf("Se fudeo otario\n");
		exit(1);
	}else{
		printf("Pacote enviado com sucesso!\n");
	}
	return;
}

void enviar(ii tabela[N_ROT], Router roteadores[N_ROT], int id_roteador){
	int r_destino;
	char msg[MSG_SIZE];
	TPacote pacote;
	printf("Digite o roteador destino: ");
	scanf("%d", &r_destino);
	printf("Digite a mensagem: ");
	scanf("%s", msg);

	pacote.origem = id_roteador;
	pacote.destino = r_destino;
	pacote.p_origem = roteadores[id_roteador].porta;
	pacote.p_destino = roteadores[r_destino].porta;
	strcpy(pacote.mensagem, msg);
	enviar_msg(tabela, roteadores, id_roteador, pacote);
}

void *receptor(void * arg){
	local_info *info = (local_info*)arg;
	struct sockaddr_in socket_addr;
	int id_roteador = info->id, sckt, s_len = sizeof(socket_addr);
	char log[MSG_SIZE], aux[2];
	aux[1] = '\0';
	Router *roteadores = info->roteadores;
	ii *tabela = info->tabela_roteamento;
	TPacote pacote;

	inicializa_socket(&socket_addr, &sckt, roteadores[id_roteador].porta);

	if( bind(sckt, (struct sockaddr *) &socket_addr, s_len) == -1){
		printf("A ligacao do socket com a porta falhou...\n");
		exit(1);
	}

	while(true){
		if(recvfrom(sckt, &pacote, sizeof(pacote), 0, (struct sockaddr *)&socket_addr, (uint *)&s_len) == -1){
			printf("putz");
		}

		if(pacote.destino != id_roteador){
			strcpy(log, "Pacote de [");
			aux[0] = (pacote.origem) + '0';
			strcat(log, aux);
			strcat(log, "] encaminhado para [");
			aux[0] = (pacote.destino) + '0';
			strcat(log, aux);
			strcat(log, "]!");
			add_log(&info->log ,log);
			enviar_msg(tabela, roteadores, id_roteador, pacote);
		}else{
			strcpy(log, "Pacote recebido de [");
			aux[0] = (pacote.origem) + '0';
			strcat(log, aux);
			strcat(log, "]!");
			add_log(&info->log ,log);
			add_msg(&info->msg, pacote.mensagem);		
		}
	}
}

void menu(){
	system("clear");
	printf("[0] Sair\n");
	printf("[1] Mostrar Tabela de Roteamento\n");
	printf("[2] Mostrar Informações dos Roteadores\n");
	printf("[3] Enviar Mensagem\n");
	printf("[4] Mostrar Mensagens Recebidas\n");
	printf("[5] Mostrar LOG\n");
}

int main(int argc, char const *argv[]){
	int id_roteador, op = 1;
	Router roteadores[N_ROT];
	ii tabela[N_ROT];
	local_info info;

	if(argc != 2){
		printf("!ERRO, os argumentos passados estão incorretos\n");
		return 0;
	}
	id_roteador = char2int(argv[1]);
	printf("%d\n", id_roteador);
	criar_tabela_roteamento(tabela, id_roteador);
	configura_roteadores(roteadores);

	info.id = id_roteador;
	info.tabela_roteamento = tabela;
	info.roteadores = roteadores;
	info.log = NULL;
	info.msg = NULL;

	pthread_create(&t_receptor, NULL, receptor, &info);
	while(op){
		menu();
		scanf("%d", &op);
		switch(op){
			case 1:
				imprimir_tabela(info.tabela_roteamento);
				break;
			case 2:
				imprimir_roteadores(info.roteadores);
				break;
			case 3:
				enviar(tabela, roteadores, id_roteador);
				break;
			case 4:
				imprimir_msg(info.msg);
				break;
			case 5:
				imprimir_log(info.log);
				break;
			default:
				break;
		}
	}
	return 0;
}