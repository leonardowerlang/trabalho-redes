# Trabalho de Redes #
Alunos: Henrique Alberto de Andrade e Leonardo Werlang
### Compilação
Compilando o arquivo da fila de prioridade:
```
$ gcc -c fila.c -o fila.o
```
Compilando o arquivo da tabela de roteamento e do roteador e renomeando o arquivo executável para roteador:
```
$ gcc tabela_roteamento.c roteador.c fila.o -o roteador -lpthread
```
### Executando o Programa
Na hora de executar o programa informe o arquivo executável e o ID do roteador.  
Exemplo:
```
$ ./roteador 1
```
### Informações Adicionais
Para alterar o enlance da rede altere o arquivo enlances.config, onde o primeiro e o segundo são valores do ID
dos roteadores e o terceior é a distância entre os roteadores.  
  
Para alterar o IP ou as portas dos roteadores acesse o arquivo roteador.config. O primeiro valor é o ID do roteador,
o segundo é sua porta e o terceiro é o IP. 
  
Para alterar a quantidade de roteadores altere a variável N_ROT do arquivo config.h  

### Exemplos de enlaces
#### Ex 1:  
roteador.config  
```
0 25001 127.0.0.1
1 25002 127.0.0.1
2 25003 127.0.0.1
3 25004 127.0.0.1
4 25005 127.0.0.1
5 25006 127.0.0.1
```
enlace.config
```
0 1 10
0 2 15
1 3 2
1 4 5
2 3 2
3 5 10
4 5 5
```
![enlace1](https://user-images.githubusercontent.com/18336694/46235327-393e8a00-c350-11e8-919b-bcc444c904f5.jpg)

#### Ex 2:  
roteador.config  
```
0 25001	127.0.0.1
1 25002 127.0.0.1
2 25003 127.0.0.1
3 25004 127.0.0.1
```
enlace.config
```
0 1 4
0 3 2
1 3 5
2 3 3
```
![enlace2](https://user-images.githubusercontent.com/18336694/46235400-87ec2400-c350-11e8-9ef7-85a2ef7493b9.jpg)

#### Ex 3:
roteador.config  
```
0 25001 127.0.0.1
1 25002 127.0.0.1
2 25003 127.0.0.1
3 25004 127.0.0.1
4 25005 127.0.0.1
5 25006 127.0.0.1
6 25007 127.0.0.1
```
enlace.config
```
0 1 2
0 2 3
1 2 2
2 3 5
3 4 3
3 5 8
4 5 2
5 6 5
```
![enlace3](https://user-images.githubusercontent.com/18336694/46235417-95a1a980-c350-11e8-8fab-4e6382faa9e0.jpg)

*Lembre de trocar o valor de N_ROT no arquivo config.h*
