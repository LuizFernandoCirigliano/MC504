#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define DIMENSAO_TABULEIRO 9
#define NUMBER_MAX_OF_THREADS 81

typedef struct posicao {
	int linha;
	int coluna;
} TipoPosicao;

typedef enum {false, true} boolean;

typedef struct dadoFinal {
	int numeroElementos;
	int numerosPossiveis[9];
	boolean verificado;
} TipoDadoFinal;

void imprime();
void* thr_verificaElementos(void*);

int** tabuleiroEntrada;
TipoDadoFinal** dadosFinais;

int main() {
	char entrada;
	int i, j, k, entradaInteira;	
	tabuleiroEntrada = (int**)malloc(sizeof(int*)*DIMENSAO_TABULEIRO);
	dadosFinais = (TipoDadoFinal**)malloc(sizeof(TipoDadoFinal*)*DIMENSAO_TABULEIRO);
	
	for(i = 0; i < DIMENSAO_TABULEIRO; i++) {
		tabuleiroEntrada[i] = (int*)malloc(sizeof(int)*DIMENSAO_TABULEIRO);
		dadosFinais[i] = (TipoDadoFinal*)malloc(sizeof(TipoDadoFinal)*DIMENSAO_TABULEIRO);	
	}

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
	pthread_t threads[NUMBER_MAX_OF_THREADS];

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

	for(i = 0; i < (entradaInteira - 1); i++) {
		pthread_join(threads[i], NULL);
	}

	imprime();

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

	for(i = 0; i < DIMENSAO_TABULEIRO; i++) {
		for(j = 0; j < DIMENSAO_TABULEIRO; j++) {
			if(maioresPossibilidades[i] < dadosFinais[j][i].numeroElementos) {
				maioresPossibilidades[i] = dadosFinais[j][i].numeroElementos;
			}
		}
	}
	
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