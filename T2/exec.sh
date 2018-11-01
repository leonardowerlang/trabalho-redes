#! /bin/bash

gcc -c funcoes.c -o funcoes.o -Wall

gcc roteador.c funcoes.o -lpthread -Wall

./a.out 1