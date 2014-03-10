/*********************************************************
*
*	Grupo:
*		Ulisses Malta Santos					140958
*		Luiz Fernando Cirigliano Villela		136734
*		José Ernesto Stelzer Monar 				139553
*
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define DIMENSAO_TABULEIRO 9
#define NUMERO_MAX_THREADS 81

/* Estrutura que armazena a posição de um elemento da matriz do sudoku. */
typedef struct posicao {
	int linha;
	int coluna;
} TipoPosicao;

typedef enum {false, true} boolean;

/* Estrutura que armazena o resultado final da verificação. */
/* Se um elemento foi verificado, armazena todos os seus possíveis valores. */
typedef struct dadoFinal {
	int numeroElementos;
	int numerosPossiveis[9];
	boolean verificado;
} TipoDadoFinal;

/* Imprime o a matriz de resultados finais. */
void imprime();
/* Função chamada por cada thread para verificar os possíveis valores de um elemento do sudoku. */
void* thr_verificaElementos(void*);

int** tabuleiroEntrada;
TipoDadoFinal** dadosFinais;

int main() {
	char entrada;
	int i, j, k, entradaInteira;
	/* Alocação das estruturas de controle. */	
	tabuleiroEntrada = (int**)malloc(sizeof(int*)*DIMENSAO_TABULEIRO);
	dadosFinais = (TipoDadoFinal**)malloc(sizeof(TipoDadoFinal*)*DIMENSAO_TABULEIRO);
	
	for(i = 0; i < DIMENSAO_TABULEIRO; i++) {
		tabuleiroEntrada[i] = (int*)malloc(sizeof(int)*DIMENSAO_TABULEIRO);
		dadosFinais[i] = (TipoDadoFinal*)malloc(sizeof(TipoDadoFinal)*DIMENSAO_TABULEIRO);	
	}
	
	/* Entrada da matriz de sudoku inicial. */
	for(i = 0; i < DIMENSAO_TABULEIRO; i++) {
		for(j = 0; j < DIMENSAO_TABULEIRO; j++) {
			do {
				scanf("%c", &entrada);
			} while(entrada == ' ' || entrada == '\n');
			entradaInteira = (int)(entrada - 48);
			if(entradaInteira > 9) {
				entradaInteira = 0;
			}
			tabuleiroEntrada[i][j] = entradaInteira;
		}
	}
	
	entradaInteira = 0;
	TipoPosicao* posicao;
	pthread_t threads[NUMERO_MAX_THREADS];
	
	/* Cada thread lançada irá verificar os possíveis valores de um elemento não preenchido da matriz inicial. */
	/* No máximo 81 threads serão lançadas, caso esse onde a matriz de sudoku inicial não tem nenhum elemento preenchido. */
	for(i = 0; i < DIMENSAO_TABULEIRO; i++) {
		for(j = 0; j < DIMENSAO_TABULEIRO; j++) {
			for(k = 0; k < 9; k++) {
				dadosFinais[i][j].numerosPossiveis[k] = 0;
			}
			if(!tabuleiroEntrada[i][j]) {
				posicao = (TipoPosicao*)malloc(sizeof(TipoPosicao));			
				posicao -> linha = i;
				posicao -> coluna = j;
				dadosFinais[i][j].numeroElementos = 0;
				dadosFinais[i][j].verificado = true;
				pthread_create(&threads[entradaInteira], NULL, thr_verificaElementos, (void*)posicao);
				entradaInteira++;
			}
			else {
				dadosFinais[i][j].verificado = false;
				dadosFinais[i][j].numeroElementos = 1;
				for(k = 0; k < 9; k++) {
					if(k != tabuleiroEntrada[i][j] - 1) {
						dadosFinais[i][j].numerosPossiveis[k] = 1;
					}
				}
			}
		}
	}
	
	/* Todas as threads devem ter sido executadas para que o programa prossiga. */
	for(i = 0; i < (entradaInteira - 1); i++) {
		pthread_join(threads[i], NULL);
	}

	imprime();
	
	/* Liberação da memória alocada durante a execução do programa. */
	for(i = 0; i < DIMENSAO_TABULEIRO; i++) {
		free(tabuleiroEntrada[i]);
		free(dadosFinais[i]);
	}

	free(tabuleiroEntrada);
	free(dadosFinais);

	return 0;
}

void* thr_verificaElementos(void* elemento) {
	int i, j;	
	TipoPosicao* posicao = (TipoPosicao*)elemento;
	
	/* Verifica quais valores cada elemento não pode ter. */
	/* No vetor numerosPossiveis da estrutura de dados final: 1 - Valor impossível / 0 - Valor possível. */
	for(i = 0; i < DIMENSAO_TABULEIRO; i++) {
		if(tabuleiroEntrada[i][posicao -> coluna]) {
			if(!dadosFinais[posicao -> linha][posicao -> coluna].numerosPossiveis[tabuleiroEntrada[i][posicao -> coluna] - 1]) {
				dadosFinais[posicao -> linha][posicao -> coluna].numerosPossiveis[tabuleiroEntrada[i][posicao -> coluna] - 1] = 1;
				dadosFinais[posicao -> linha][posicao -> coluna].numeroElementos++;
			}
		}
		if(tabuleiroEntrada[posicao -> linha][i]) {
			if(!dadosFinais[posicao -> linha][posicao -> coluna].numerosPossiveis[tabuleiroEntrada[posicao -> linha][i] - 1]) {
				dadosFinais[posicao -> linha][posicao -> coluna].numerosPossiveis[tabuleiroEntrada[posicao -> linha][i] - 1] = 1;
				dadosFinais[posicao -> linha][posicao -> coluna].numeroElementos++;
			}
		}
	}
	
	for(i = ((posicao -> linha)/3)*3; i < ((((posicao -> linha)/3) + 1) * 3); i++) {
		for(j = ((posicao -> coluna)/3)*3; j < ((((posicao -> coluna)/3) + 1) * 3); j++) {
			if(tabuleiroEntrada[i][j]) {
				if(!dadosFinais[posicao -> linha][posicao -> coluna].numerosPossiveis[tabuleiroEntrada[i][j] - 1]) {
					dadosFinais[posicao -> linha][posicao -> coluna].numerosPossiveis[tabuleiroEntrada[i][j] - 1] = 1;
					dadosFinais[posicao -> linha][posicao -> coluna].numeroElementos++;
				}
			}
		}        	
	}
	
	/* Armazena o número de valores possíveis para cada elemento não preenchido do sudoku. */ 
	dadosFinais[posicao -> linha][posicao -> coluna].numeroElementos = 9 - dadosFinais[posicao -> linha][posicao -> coluna].numeroElementos;

	free(posicao);

	return NULL;
}

void imprime(){
	int i, j, k, l;
	int maioresPossibilidades[9];
		
	for(i = 0; i < DIMENSAO_TABULEIRO; i++) {
		maioresPossibilidades[i] = 0;
	}
	
	/* Verifica qual a maior quantidade de dígitos que devem ser impressos em cada coluna. */
	for(i = 0; i < DIMENSAO_TABULEIRO; i++) {
		for(j = 0; j < DIMENSAO_TABULEIRO; j++) {
			if(maioresPossibilidades[i] < dadosFinais[j][i].numeroElementos) {
				maioresPossibilidades[i] = dadosFinais[j][i].numeroElementos;
			}
		}
	}
	
	/* Imprime a matriz final com seus elementos alinhados. */
	for(i = 0; i < DIMENSAO_TABULEIRO; i++) {
		for(j = 0; j < DIMENSAO_TABULEIRO; j++) {
			if(dadosFinais[i][j].verificado) {
				printf("(");
				for(k = 0; k < 9; k++) {
					if(!dadosFinais[i][j].numerosPossiveis[k]) {
						printf("%d", k + 1);
					}
				}
				printf(")");
				l = (maioresPossibilidades[j]+1-dadosFinais[i][j].numeroElementos);
			}
			else {
				for(k = 0; k < 9; k++) {
					if(!dadosFinais[i][j].numerosPossiveis[k]) {
						printf(" %d", k + 1);
						break;
					}
				}
				l = (maioresPossibilidades[j]+2-dadosFinais[i][j].numeroElementos);
			}
			if(j != (DIMENSAO_TABULEIRO - 1)) {			
				for(k = 0; k < l; k++) {
					printf(" ");
				}
			}			
		}
		printf("\n");		
	}
}