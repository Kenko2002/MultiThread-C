#pragma once
#define _CRT_SECURE_NO_WARNINGS 1 
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1

#pragma comment(lib,"pthreadVC2.lib")

#define HAVE_STRUCT_TIMESPEC 
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*Defini��o da quantidade de linhas e colunas de uma matriz N x M*/

#define LINHA 15000 
#define COLUNA 15000 

/*Defini��o da quantidade de linhas e colunas do macrobloco*/

#define LIHNA_MACROBLOCO 100
#define COLUNA_MACROBLOCO 100

/*Defini��o do numero de macroblocos por linha e coluna*/

#define QTD_MACR0BLOCO_LINHAS (LINHA / LIHNA_MACROBLOCO) 
#define QTD_MACR0BLOCO_COLUNAS (COLUNA / COLUNA_MACROBLOCO) 

/*Defini��o dos valores de verdadeiro (1) e falso (0) */

#define FALSE 0
#define TRUE 1

/*Defini��o da semente de gera��o de n�meros rand�micos*/

#define SEED 200

/*Defini��o da quantidade de Threads implementadas*/

#define NUM_THREADS 8

/*Vari�veis globais (matriz e contador de n�meros primos)*/

int** matriz;
int contadorPrimos = 0;

/*Matriz que represente as regi�es de cada macrobloco, todas iniciadas com o valor 0*/

int** matrizRegioes;

/*Vari�veis globais mutex*/

pthread_mutex_t contadorMutex;
pthread_mutex_t macroblocoMutex;

/*Fun��es usadas*/

int ehPrimo(int numero);
int** alocarMatriz();
int** desalocarMatriz();
int** alocarMatrizRegioes();
int** desalocarMatrizRegioes();
void contagemSerial();
void contagemParalela();
void* contador(void*);

int main(int argc, char* argv[])
{

	double tempo;/*Tempo marcado de cada contagem*/
	clock_t tInicio, tFim;/*Cronometros de in�cio e fim*/

	matriz = alocarMatriz(); /*Aloca a mem�ria para a matriz, inserindo os valores randomicos de 0 31999*/
	matrizRegioes = alocarMatrizRegioes(); /*Aloca a mem�ria para a matriz de regi�es de cada macrobloco, inserindo o valor 0*/

	tInicio = clock(); /*Inicia o cron�metro*/

	contagemSerial(); /*Inicia a fun��o de contagem do estilo Serial*/

	tFim = clock(); /*Termina o cron�metro*/

	tempo = (double)(tFim - tInicio) / CLOCKS_PER_SEC; /*Valor de segundos do primeiro tempo marcado*/

	printf("Quantidade de elementos primos na matriz por busca serial: %d\n", contadorPrimos);
	printf("Tamanho da matriz: %d X %d\n", LINHA, COLUNA);
	printf("Tempo total de contagem: %f segundos\n\n", tempo);

	contadorPrimos = 0; /*Zera o contador de n�meros primos*/

	tInicio = clock(); /*Inicia o cron�metro*/

	contagemParalela(); /*Inicia a fun��o de contagem do estilo Paralelo*/

	tFim = clock(); /*Termina o cron�metro*/

	tempo = (double)(tFim - tInicio) / CLOCKS_PER_SEC; /*Valor de segundos do primeiro tempo marcado*/

	printf("Quantidade de elementos primos na matriz por busca paralela: %d\n", contadorPrimos);
	printf("Tamanho da matriz: %d X %d\n", LINHA, COLUNA);
	printf("Tamanho do macrobloco: %d X %d\n", LIHNA_MACROBLOCO, COLUNA_MACROBLOCO);
	printf("Numero de threads: %d\n", NUM_THREADS);
	printf("Tempo total de contagem: %f segundos\n\n", tempo);

	matriz = desalocarMatriz(); /*Desaloca a mem�ria da matriz*/
	matrizRegioes = desalocarMatrizRegioes(); /*Desaloca a mem�ria da matriz de regi�es*/

	system("PAUSE");

	return 0;

}


/*Fun��o que retorna se o valor � primo ou n�o*/

int ehPrimo(int numero)
{

	int i;

	/*Se o n�mero for menor que 2, retorna falso*/

	if (numero < 2) return FALSE;

	/*Dos valores 2 a raiz do n�mero, se o resto do n�mero pelo valor for
	igual a zero, retorna falso*/

	for (i = 2; i <= sqrt(numero); i++)
	{
		if (numero % i == 0) return FALSE;
	}

	/*Caso nada aconteceu, retorna verdadeiro*/

	return TRUE;

}

/*Fun��es de aloca��o e desaloca��o de valores de mem�ria da matriz*/

int** alocarMatriz()
{

	int** matriz; /*Instancia um valor local para matriz*/
	int i, j;

	/*Inicia o gerador de n�meros rand�micos*/

	srand(SEED);

	/*Faz a aloca��o din�mica de um vetor do tamnho da linha, com
	ponteiro de inteiros*/

	matriz = malloc(LINHA * sizeof(int*));

	/*Caso a matriz for nulo, ouve erro na aloca��o*/

	if (matriz == NULL)
	{
		printf("** Erro: Mem�ria Insuficiente **");
		return NULL;
	}

	/*Para cada valor desse vetor, se faz a aloca��o din�mica do tamanho de
	colunas da matriz. Caso der nulo um dos ponteiros, ouve erro de aloca��o*/

	for (i = 0; i < LINHA; i++)
	{

		matriz[i] = malloc(COLUNA * sizeof(int));

		if (matriz[i] == NULL)
		{
			printf("** Erro: Mem�ria Insuficiente **");
			return NULL;
		}

	}

	/*Para cada elemento na matriz, instancia um valor rand�mico de
	0 a 31999*/

	for (i = 0; i < LINHA; i++)
	{
		for (j = 0; j < COLUNA; j++)
		{
			matriz[i][j] = (rand() % 32000);
		}
	}

	return matriz; /*Retorna a matriz*/

}

int** desalocarMatriz()
{

	int i;

	if (matriz == NULL) return NULL; /*Caso a matriz for nula, retorna nulo*/

	for (i = 0; i < LINHA; i++) free(matriz[i]); /*Para cada elemento no vetor de linhas da matriz, de um free()*/

	free(matriz); /*De um free() no vetor de linhas*/

	return NULL; /*Retorne nulo*/

}

/*Fun��es de aloca��o e desaloca��o de valores de mem�ria da matriz de regi�es*/

int** alocarMatrizRegioes()
{

	int** matrizRegioes; /*Instancia um valor local para matriz de regi�es*/
	int i, j;

	/*Faz a aloca��o din�mica de um vetor do tamnho da linha, com
	ponteiro de inteiros*/

	matrizRegioes = malloc(QTD_MACR0BLOCO_LINHAS * sizeof(int*));

	/*Caso o mat for nulo, ouve erro na aloca��o*/

	if (matrizRegioes == NULL)
	{
		printf("** Erro: Mem�ria Insuficiente **");
		return NULL;
	}

	/*Para cada valor desse vetor, se faz a aloca��o din�mica do tamanho de
	colunas da matriz. Caso der nulo um dos ponteiros, ouve erro de aloca��o*/

	for (i = 0; i < QTD_MACR0BLOCO_LINHAS; i++)
	{

		matrizRegioes[i] = malloc(QTD_MACR0BLOCO_COLUNAS * sizeof(int));

		if (matrizRegioes[i] == NULL)
		{
			printf("** Erro: Mem�ria Insuficiente **");
			return NULL;
		}

	}

	/*Para cada elemento na matriz de regi�es, iniciar com o valor 0*/

	for (i = 0; i < QTD_MACR0BLOCO_LINHAS; i++)
	{
		for (j = 0; j < QTD_MACR0BLOCO_COLUNAS; j++)
		{
			matrizRegioes[i][j] = 0;
		}
	}

	return matrizRegioes; /*Retorna a matriz de regi�es*/

}

int** desalocarMatrizRegioes()
{

	int i;

	if (matrizRegioes == NULL) return NULL; /*Caso a matriz de regi�es for nula, retorna nulo*/

	for (i = 0; i < QTD_MACR0BLOCO_LINHAS; i++) free(matrizRegioes[i]); /*Para cada elemento no vetor de linhas da matriz, de um free()*/

	free(matrizRegioes); /*De um free() no vetor de linhas*/

	return NULL; /*Retorne nulo*/

}

/*Fun��es de contagem do modo Serial e Paralelo*/

void contagemSerial()
{

	int i, j;

	/*Para cada elemento presente na matriz, se o elemento
	for primo, soma 1 ao contador de n�meros primos*/

	for (i = 0; i < LINHA; i++)
	{
		for (j = 0; j < COLUNA; j++)
		{
			if (ehPrimo(matriz[i][j])) contadorPrimos++;
		}
	}

}

void contagemParalela()
{

	int i;

	pthread_mutex_init(&contadorMutex, NULL); /*Inicia o mutex*/
	pthread_mutex_init(&macroblocoMutex, NULL); /*Inicia o mutex*/

	pthread_t tid[NUM_THREADS]; /*Vetor de indentificadores da Thread*/

	for (i = 0; i < NUM_THREADS; i++)
	{

		if (pthread_create(&(tid[i]), NULL, contador, NULL) != 0)
		{
			perror("Pthread_create falhou!");
			exit(1);
		}
	}

	/*Join nas Threads*/

	for (i = 0; i < NUM_THREADS; i++)
	{
		if (pthread_join((tid[i]), NULL) != 0)
		{
			perror("Pthread_join falhou!");
			exit(1);
		}

	}

	pthread_mutex_destroy(&contadorMutex); /*Destroi o mutex*/
	pthread_mutex_destroy(&macroblocoMutex); /*Destroi o mutex*/

}

/*Fun��o usada pelas Threads para a contagem de valores primos*/

void* contador(void* arg)
{

	int contadorLocal = 0; /*Inicia um contador de n�meros primos local*/

	/*Se faz um la�o for para a leitura compartilhada entre as threads da matriz de regi�es de macroblocos*/

	for (int macroLinhas = 0; macroLinhas < QTD_MACR0BLOCO_LINHAS; macroLinhas++)
	{
		for (int macroColunas = 0; macroColunas < QTD_MACR0BLOCO_COLUNAS; macroColunas++)
		{

			pthread_mutex_lock(&macroblocoMutex); /*Faz o lock (regi�o cr�tica que faz modifica��o na matriz de regi�es)*/

			if (matrizRegioes[macroLinhas][macroColunas]) /*Caso o valor da matriz for igual a 1, significa que esse macrobloco j� foi lido*/
			{
				pthread_mutex_unlock(&macroblocoMutex);

				continue; /*Pula para o pr�ximo valor (j+1)*/
			}

			matrizRegioes[macroLinhas][macroColunas] = 1; /*Caso o valor for 0, adiciona o valor 1 na matriz representando que essa regi�o foi lida*/

			pthread_mutex_unlock(&macroblocoMutex); /*Destrava o mutex da regi�o cr�tica */

			int linhaInicial = macroLinhas * LIHNA_MACROBLOCO; /*Linha inicial do macrobloco*/
			int linhaFinal = linhaInicial + LIHNA_MACROBLOCO; /*Linha final do macrobloco*/
			int colunaInicial = macroColunas * COLUNA_MACROBLOCO; /*Coluna inicial do macrobloco*/
			int colunaFinal = colunaInicial + COLUNA_MACROBLOCO; /*Coluna final do macrobloco*/

			/*Leitura do macrobloco, somando ao contador local quando o valor for primo*/

			for (int i = linhaInicial; i < linhaFinal; i++)
			{
				for (int j = colunaInicial; j < colunaFinal; j++)
				{
					if (ehPrimo(matriz[i][j]))
					{
						contadorLocal++;
					}
				}
			}
		}
	}

	pthread_mutex_lock(&contadorMutex); /*Faz o lock (regi�o cr�tica que acrescenta o valor local ao valor global)*/
	contadorPrimos += contadorLocal;
	pthread_mutex_unlock(&contadorMutex); /*Destrava o mutex da regi�o cr�tica */

	pthread_exit(0); /*Ternina a Thread, retornando 0*/

} 