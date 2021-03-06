#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#define NUMERO_THREADS 4

int GlobalThreadCount = 0, resultFound = 0;
/*Funcao que imprime a matriz 9x9 que armazena o jogo de Sudoku*/
void imprime(short **mat){
	int i, j;
	for(i=0; i<9; i++){
		if(i%3 == 0)
			printf("--------------------------\n");
		for(j=0; j<9; j++){
			if(j%3 == 0)
				printf(" |");
			printf(" %d", mat[i][j]);
		}
		printf(" |\n");
	}
	printf("--------------------------\n");
	return;
}

void checa(short **mat,int *possivel,int linha, int coluna) {
	int i,j,bloco_l, bloco_c;
	for(i=1;i<10;i++) {
		possivel[i] = 1;
	}
	for(i=0;i<9;i++) {
		possivel[mat[linha][i]] = 0;
		possivel[mat[i][coluna]] = 0;
	}
	bloco_l = (linha/3)*3;
	bloco_c = (coluna/3)*3;
	for(i=bloco_l; i<bloco_l + 3; i++)
		for(j=bloco_c; j<bloco_c + 3; j++) {
			possivel[mat[i][j]] = 0;
		}
}

int completo (short **mat){
	int i,j;
	for(i=0;i<9;i++) {
		for(j=0;j<9;j++) {
			if(mat[i][j] == 0)
				return 0;
		}
	}
	return 1;
}


void * sudoku(void *mat){
	int i,j = 0,k,x,y,stop = 0, threadCount = 0;
	int *retorno = (int *) malloc (sizeof (int));
    int resultado;
	short **matCopy[9], **matriz;
	int possivel[10];
    
    pthread_t thr[9] = {0};
    
    if (resultFound) {
    	*retorno = 0;
    	return (void *) retorno;
    }
	matriz = (short **) mat;
    //testa se tabuleiro esta completo
	if(completo(matriz)) {
		resultFound = 1;
		imprime(matriz);

		exit(0);

		*retorno = 1;
		return (void *)retorno;
	}

	//acha primeira posicao vazia no tabuleiro
	for(i=0;i<9 && !stop;i++) {
		for(j=0;j<9 && !stop;j++) {
			if(matriz[i][j] == 0)
				stop = 1;
		}
	}
	if(i>0) i--; if(j>0) j--;
	checa(matriz,possivel, i, j);
    
	for(k=1;k<10;k++) {
		matCopy[k-1] = NULL;
		//testa se o numero pode ser colocado no quadrado
		if(possivel[k]) {
			//aloca matriz auxiliar para copiar valores para proxima decisao
            matCopy[k-1] = (short **)malloc(sizeof(short *) * 9);
			for (x = 0; x < 9; x++) {
				matCopy[k-1][x] = (short *)malloc(sizeof(short)* 9);
			}
			
			//copia valores (tentar trocar por memcpy dps)
			for (x = 0; x < 9 ; x++)
				for (y = 0 ; y < 9 ; y ++)
					matCopy[k-1][x][y] = matriz[x][y];
            //memcpy (matCopy[k-1], matriz, sizeof(short)*81);
			//insere tentativa de valor
			matCopy[k-1][i][j] = k;

			//desce um nivel na recursao
			if (GlobalThreadCount >= NUMERO_THREADS) {
				resultado = *((int *) sudoku(matCopy[k - 1]));
				//se resultado == 1, o sudoku foi completado, desfaz recursao
				//matCopy[k-1][i][j] = 0;

				if(resultado) {
					*retorno = 1;

					//libera matrizes auxiliares
					for (x = 0; x < k  ; x++) {
							if (matCopy[x]) {
								for (y= 0; y < 9; y++) {
									free(matCopy[x][y]);
								}

								free (matCopy[x]);
							}
					}
					
			
					return (void *)retorno;
				}
			}
			else {
				//imprime(matCopy[k - 1]);
				GlobalThreadCount++;
				pthread_create(&thr[threadCount++], NULL, sudoku, (void *)matCopy[k-1]);
				//printf("nova thread \n");
			}
		}
	}
	*retorno = 0;

	for (i = 0; i < threadCount; i++) {
		if (thr[i]) {
			pthread_join(thr[i], (void *) &resultado);
			GlobalThreadCount--;
			if (resultado == 1)
				*retorno = 1;
		}
	}
	
	
	//libera matrizes auxiliares
	for (i = 0; i < 9 ; i++) {
		if (matCopy[i]) {
			for ( x= 0; x < 9; x++) {
				free(matCopy[i][x]);
			}
			free (matCopy[i]);
		}
	}
	
	return (void *)retorno;
}

int main(){
	short **mat;
	int X,Y;
	int i,j;
	int resultado;
	short entradaInteira;
    char entrada;
	pthread_t thr;
	
	//aloca matriz    
	mat = (short **)malloc(sizeof(short *) * 9);
	for (i = 0; i < 9; i++) {
		mat[i] = (short *)malloc(sizeof(short)* 9);
	}
	//zera matriz
	for (X = 0; X < 9; X++) 
		for (Y = 0; Y < 9 ; Y++)
			mat[X][Y] = 0;
	//le entradas
		/* Entrada da matriz de sudoku inicial. */
	for(i = 0; i < 9; i++) {
		for(j = 0; j < 9; j++) {
			do {
				scanf("%c", &entrada);
			} while(entrada == ' ' || entrada == '\n');
			entradaInteira = (short)(entrada - 48);
			if(entradaInteira > 9) {
				entradaInteira = 0;
			}
			mat[i][j] = entradaInteira;
		}
	}
	//chama thread
	pthread_create(&thr, NULL, sudoku, (void *)mat);
	 pthread_join (thr, (void *)&resultado);
	
	
	printf("Sem solucao\n");
	
	return 0;
}
