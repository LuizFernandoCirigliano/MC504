/**********************************************************************
*
*	Grupo:
*		Ulisses Malta Santos					140958
*		Luiz Fernando Cirigliano Villela		136734
*		José Ernesto Stelzer Monar 				139553
*
**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUMERO_THREADS 9

typedef enum {false, true} boolean;

int matriz[9][9] = {{0}};
boolean correto = true;

/* Verifica se uma linha possui elementos repetidos. */
void* verificaLinha(void* linhaAp) {
	int numeros[9] = {0};
	int i, linha = *(int*)linhaAp;

	for(i = 0; i < 9; i++) {
		numeros[matriz[linha][i]-1]++;	
	}

	for(i = 0; i < 9; i++) {
		if(numeros[i] > 1) {
			printf("A linha %d possui %d ocorrências do número %d.\n", linha + 1, numeros[i], i + 1);
			correto = false;
		}
	}

	return NULL;
}

/* Verifica se uma coluna possui elementos repetidos. */
void* verificaColuna(void* colunaAp) {
	int numeros[9] = {0};
	int i, coluna = *(int*)colunaAp;

	for(i = 0; i < 9; i++) {
		numeros[matriz[i][coluna]-1]++;	
	}

	for(i = 0; i < 9; i++) {
		if(numeros[i] > 1) {
			printf("A coluna %d possui %d ocorrências do número %d.\n", coluna + 1, numeros[i], i + 1);
			correto = false; 
		}
	}

	return NULL;
}

/* Verifica se um quadrante 3 x 3 da matriz de sudoku tem elementos repetidos. */
void* verificaBloco(void* bloco) {
	int i, j;	
	int numeros[9] = {0};	
	int numBloco = *(int*)bloco;

    for(i = (numBloco/3)*3; i < (((numBloco/3) + 1) * 3); i++) {
		for(j = (numBloco%3)*3; j < (((numBloco%3) + 1) * 3); j++) {
			numeros[matriz[i][j]-1]++;
		}        	
	}
	
	for(i = 0; i < 9; i++) {
		if(numeros[i] > 1) {
			printf("O bloco %d possui %d ocorrências do número %d.\n", numBloco + 1, numeros[i], i + 1);
			correto = false;
		}
	}
	
	return NULL;
}

int main() {
	int i, j, *thread_id;	
	pthread_t thr[NUMERO_THREADS];
	
	for(i = 0; i < 9; i++) {
		for(j = 0; j < 9; j++) {
			scanf("%d", &matriz[i][j]);
		}
	}
	
	/* Cada thread verifica uma linha, uma coluna e um quadrante da matriz de sudoku. */
	for(i = 0; i < NUMERO_THREADS; i++) {
		thread_id = (int*)malloc(sizeof(int));
		*thread_id = i;		
	    pthread_create(&thr[i], NULL, verificaLinha, (void*) thread_id);
	    pthread_create(&thr[i], NULL, verificaColuna, (void*) thread_id);
	    pthread_create(&thr[i], NULL, verificaBloco, (void*) thread_id);
	}
	
	/* Aguarda a execução das threads para que  o programa continue. */
	for(i = 0; i < NUMERO_THREADS; i++) {
	    pthread_join(thr[i], NULL);
	}
	
	if(correto) {
		printf("O sudoku está correto!\n");
	}

	return 0;
}
