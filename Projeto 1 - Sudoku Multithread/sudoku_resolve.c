/**********************************************************
*
*	Grupo:
*		Ulisses Malta Santos							140958
*		Luiz Fernando Cirigliano Villela			136734
*		José Ernesto Stelzer Monar					139553
*
**********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define NUMERO_THREADS 9

/* Função que imprime a matriz 9x9 que armazena o jogo de Sudoku (opcionalmente a matriz pode ser global e não passada por parâmetro). */
void imprime(short** mat) {
	int i, j;
	for(i = 0; i < 9; i++) {
		for(j = 0; j < 9; j++) {
			printf("%d ", mat[i][j]);
		}
		printf("\n");
	}
}

void checa(short** mat, int* possivel, int linha, int coluna) {
	int i, j, bloco_l, bloco_c;

	for(i = 1; i < 10; i++) {
		possivel[i] = 1;
	}

	for(i = 0; i < 9; i++) {
		possivel[mat[linha][i]] = 0;
		possivel[mat[i][coluna]] = 0;
	}
	
	bloco_l = (linha/3)*3;
	bloco_c = (coluna/3)*3;

	for(i = bloco_l; i < bloco_l + 3; i++) {
		for(j = bloco_c; j < bloco_c + 3; j++) {
			possivel[mat[i][j]] = 0;
		}
	}
}

int completo(short** mat) {
	int i, j;
	for(i = 0; i < 9; i++) {
		for(j = 0; j < 9; j++) {
			if(mat[i][j] == 0) {
				return 0;
			}
		}
	}
	return 1;
}

void* sudoku(void* mat){
	int i, j = 0, k, x, y, stop = 0;
	int* retorno = (int*)malloc(sizeof(int));
	int* resultado;
	short **matCopy[9], **matriz;
	int possivel[10];
   
	pthread_t thr[NUMERO_THREADS];
    
	matriz = (short**)mat;
    	
	/* Verifica se a matriz está completa. */
	if(completo(matriz)) {
		imprime(matriz);
		*retorno = 1;
		return (void*)retorno;
	}

	/* Acha a primeira posição vazia no tabuleiro de sudoku. */
	for(i = 0; i < 9 && !stop; i++) {
		for(j = 0; j < 9 && !stop; j++) {
			if(matriz[i][j] == 0)
				stop = 1;
		}
	}

	if(i > 0) i--; 
	if(j > 0) j--;
	
	checa(matriz, possivel, i, j);
    
	for(k = 1; k < 10; k++) {
		matCopy[k-1] = NULL;
		
		/* Testa se o numero pode ser colocado no quadrado. */
		if(possivel[k]) {
			/* Aloca matriz auxiliar para copiar valores para próxima decisão. */
			matCopy[k-1] = (short**)malloc(sizeof(short*)*9);

			for(x = 0; x < 9; x++) {
				matCopy[k-1][x] = (short*)malloc(sizeof(short)*9);
			}
			
			/* Copia os valores. */
			for(x = 0; x < 9; x++) {
				for(y = 0; y < 9; y++) {
					matCopy[k-1][x][y] = matriz[x][y];
				}
			}
            
			/* Insere tentativa de valor. */
			matCopy[k-1][i][j] = k;

			/* Desce um nível na recursão. */
			resultado = (int*)sudoku(matCopy[k-1]);

			/* Se resultado == 1, o sudoku foi completado, desfaz recursão. */
			if(*resultado) {
				*retorno = 1;

				/* Libera as matrizes auxiliares. */
				for(x = 0; x < k; x++) {
					if(matCopy[x]) {
						for(y= 0; y < 9; y++) {
							free(matCopy[x][y]);
						}
						free (matCopy[x]);
					}
				}
				free (resultado);
		
				return (void*)retorno;
			} 
			else if(resultado) {
				free (resultado);
			}
			matCopy[k-1][i][j] = 0;
		}
	}    
	
	*retorno = 0;
	
	/* Libera as matrizes auxiliares. */
	for(i = 0; i < 9; i++) {
		if(matCopy[i]) {
			for(x= 0; x < 9; x++) {
				free(matCopy[i][x]);
			}
			free (matCopy[i]);
		}
	}
	
	return (void*)retorno;
}

int main() {
	short** mat;
	char entrada;
	int i, j, resultado;

	pthread_t thr[NUMERO_THREADS];

	/* Alocação da matriz do sudoku. */   
	mat = (short**)malloc(sizeof(short*)*9);
	for(i = 0; i < 9; i++) {
		mat[i] = (short*)malloc(sizeof(short)*9);
	}

	/* Entrada da matriz de sudoku inicial. */
	for(i = 0; i < 9; i++) {
		for(j = 0; j < 9; j++) {
			do {
				scanf("%c", &entrada);
			} while(entrada == ' ' || entrada == '\n');
		
			if((int)entrada - 48 > 9) {
				mat[i][j] = 0;
			}
			else {
				mat[i][j] = (int)entrada - 48;
			}			
		}
	}
	
	/* Chamada das threads. */
	pthread_create(&thr[0], NULL, sudoku, (void*) mat);
	pthread_join (thr[0], (void*) &resultado);
	
	if(!resultado) {
		printf("Sem solução!\n");
	}

	/* Libera a memória alocada. */
	for (i = 0; i < 9; i++) {
		free(mat[i]);
	}
	free(mat);

	return 0;
}
