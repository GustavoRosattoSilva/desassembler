# Projeto de Tradução de Linguagem de Máquina para Assembly IA-32

## Autores

- Alexandre Ferreira - 120025175
- Gustavo Rosatto Silva - 190042818

## Descrição

Este projeto consiste na implementação de um programa em C que traduz um código de linguagem de máquina simples para o assembly IA-32.

## Sistema operacional
O sistema operacional utilizado para o seu desenvolvimento foi o Linux Mint 21.3

## Compilação

O código foi compilado utilizando o GCC (GNU Compiler Collection). Para compilar o projeto, utilize o seguinte comando:

```bash
gcc -Wall main.c -o tradutor
```

ou

```bash
make
```

Para executar informar um arquivo do tipo .obj como entrada

```bash
./tradutor <myfile>.obj
```