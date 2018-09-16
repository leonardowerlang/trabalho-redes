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

void enviar_msg(ii tabela[], Router roteadores[], int id_roteador){
	struct sockaddr_in socket_addr;
	int r_destino, sckt, s_len = sizeof(socket_addr);
	char msg[MSG_SIZE];
	TPacote pacote;

	if ((sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
		printf("ERRO! Socket\n");
		exit(1);
	}


	printf("Digite o roteador destino: ");
	scanf("%d", &r_destino);
	printf("Digite a mensagem: ");
	scanf("%s", msg);

	memset((char *) &socket_addr, 0, sizeof(socket_addr));
	socket_addr.sin_family = AF_INET;
	socket_addr.sin_port = htons(roteadores[r_destino].porta);

	pacote.origem = id_roteador;
	pacote.destino = r_destino;
	strcpy(pacote.mensagem, msg);
	if(inet_aton(roteadores[r_destino].ip , &socket_addr.sin_addr) == 0){
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
	int id_roteador = info->id, sckt, s_len = sizeof(socket_addr);;
	Router *roteadores = info->roteadores;
	ii *tabela = info->tabela_roteamento;
	TPacote package;


	/*printf("Teste: %d\n", id_roteador);

	for(int i = 0; i < N_ROT; i++){
		printf("ID1: %d P: %d IP: %d\n", i, tabela[i].v, tabela[i].distancia);
	}*/
	if((sckt = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1){
        printf("Falha ao criar Socket!");
        exit(1);
    }else{
        memset((char *) &socket_addr, 0, s_len);
        socket_addr.sin_family = AF_INET;
        socket_addr.sin_port = htons(roteadores[id_roteador].porta); //Porta em ordem de bytes de rede
        socket_addr.sin_addr.s_addr = htonl(INADDR_ANY); //Atribui o socket a todo tipo de interface
    }

    if( bind(sckt, (struct sockaddr*) &socket_addr, s_len) == -1){
        printf("A ligacao do socket com a porta falhou...\n");
        exit(1);
    }

    while(1){
		if(recvfrom(sckt, &package, sizeof(TPacote), 0, (struct sockaddr*)&socket_addr, &s_len) == -1){
			printf("putz");
		}

		printf("Teste: %s\n", package.mensagem);
    }
}

int main(int argc, char const *argv[]){
	int id_roteador;
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

	pthread_create(&t_receptor, NULL, receptor, &info);

	// for(int i = 0; i < N_ROT; i++){
	// 	printf("ID: %d P: %d IP: %s\n", i, roteadores[i].porta, roteadores[i].ip);
	// }

	while(1){
		enviar_msg(tabela, roteadores, id_roteador);
	}
	printf("asfasf\n");

	return 0;
}