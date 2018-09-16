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

typedef struct{
	Router *roteadores;
	ii *tabela_roteamento;
	int id;
}local_info;

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

void *receptor(void * arg){
	local_info *info = (local_info*)arg;
	struct sockaddr_in socket_addr;
	int id_roteador = info->id, sckt, s_len = sizeof(socket_addr);
	Router *roteadores = info->roteadores;
	ii *tabela = info->tabela_roteamento;
	TPacote package;

	inicializa_socket(&socket_addr, &sckt, roteadores[id_roteador].porta);

	if( bind(sckt, (struct sockaddr*) &socket_addr, s_len) == -1){
		printf("A ligacao do socket com a porta falhou...\n");
		exit(1);
	}

	while(1){
		if(recvfrom(sckt, &package, sizeof(TPacote), 0, (struct sockaddr*)&socket_addr, (uint *)&s_len) == -1){
			printf("putz");
		}

		if(package.destino != id_roteador){
			printf("\nMensagem de [%d] encaminhada para [%d]\n", package.origem, package.destino);
			enviar_msg(tabela, roteadores, id_roteador, package);
		}else{
			printf("\nMensagem recebida de [%d] enviada pela porta [%d]\n", package.origem, package.p_origem);
			printf("A mensagem foi enviada para [%d] recebidad pela porta [%d]\n", package.destino, package.p_destino);
			printf("Mensagem: %s\n", package.mensagem);		
		}

	}
}

int main(int argc, char const *argv[]){
	int id_roteador, r_destino;
	char msg[MSG_SIZE];
	Router roteadores[N_ROT];
	ii tabela[N_ROT];
	local_info info;
	TPacote pacote;

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

	pthread_create(&t_receptor, NULL, receptor, &info);

	// for(int i = 0; i < N_ROT; i++){
	// 	printf("ID: %d P: %d IP: %s\n", i, roteadores[i].porta, roteadores[i].ip);
	// }

	while(1){

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
	printf("asfasf\n");

	return 0;
}