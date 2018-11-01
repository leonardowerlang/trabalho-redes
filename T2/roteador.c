#include "funcoes.h"

int main(int argc, char const *argv[]){
	LocalInfo info;

	if(argc != 2){
		printf("ERRO! Informe o arquivo executável e o número do roteador.\n");
		return 0;
	}

	inicializaRoteador(&info, char2int(argv[1]));
}