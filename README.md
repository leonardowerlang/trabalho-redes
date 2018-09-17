# Trabalho Redes #

### Compilação
Compilando o arquivo da fila de prioridade:
```
$ gcc -c fila.c -o fila.o
```
Compilando o arquivo da tabela de roteamento e do roteador e renomeando o arquivo executavel para roteador:
```
$ gcc tabela_roteamento.c roteador.c fila.o -o roteador -lpthread
```
### Executando o Programa
Na hora de executar o programa informe o arquivo executavel e o id do roteador.  
Ex:
```
$ ./roteador 1
```
### Informações Adicionais
Para alterar o enlance da rede altere o arquivo enlances.config, onde o primeiro e o segundo são valores do id
dos roteadores e o terceior é a distancia entre os roteadores.  
  
Para alterar o ip ou as portas dos roteadores acesse o arquivo roteador.config. O primeiro valor é o id do roteador,
o segundo é sua porta e o terceiro é o ip. 
  
Para alterar a quantidade de roteadores altere a variável N_ROT do arquivo config.h
