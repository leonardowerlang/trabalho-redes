#ifndef FUNCOES_H
#define FUNCOES_H

#include "config.h"

void menu();
Roteador *getRoteador(Roteador *r, int id);
void popListaEspera(ListaEspera **lista, pthread_mutex_t *mutex);
void pushListaEspera(ListaEspera **lista, Pacote pacote, pthread_mutex_t *mutex);
void imprimirRoteadores(Roteador *r);
void imprimirTopologia(Topologia *t);
int char2int(char const *str);
void imprimirPacote(Pacote *pacote);
void imprimirLista(ListaEspera *lista);
void inicializaSocket(struct sockaddr_in *socket_addr, int *sckt, int porta);
void lerRoteadores(LocalInfo *info);
void inicializaRoteador(LocalInfo *info, int id);
void pushLog(Log **log, char *msg, pthread_mutex_t *mutex);
void imprimirMSG(Log *log);

#endif