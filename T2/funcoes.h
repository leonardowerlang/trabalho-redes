#include "config.h"

void imprimirRoteadores(Roteador *r);
void imprimirTopologia(Topologia *t);
int char2int(char const *str);
void inicializaSocket(int *sockfd, struct sockaddr_in *serv_addr, int porta);
void lerRoteadores(LocalInfo *info);
void inicializaRoteador(LocalInfo *info, int id);