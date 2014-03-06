#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#define N_THR 9

int matriz [9][9] = {{0}};

void * verificaLinha (void *linhaAp) {
	int numeros[9] = {0};
	int i , linha = *(int *)linhaAp;
	for (i = 0 ; i < 9 ; i ++) {
		if ( numeros[ matriz [linha][i] - 1] == 1) {
			//errado
			printf("erro na linha %d no numero %d \n", linha + 1 , matriz [linha][i]);
			return NULL;
		} else {
			numeros[ matriz [linha][i] - 1] = 1; 
		}	
	}	
	return NULL;
}

void * verificaBloco (void *bloco) {
	int numeros[9] = {0} ;
	int i,j,bloco_l, bloco_c;
	int numBloco = *(int *)bloco;

	bloco_l = (numBloco / 3) * 3;
    bloco_c = (numBloco % 3) * 3;

    for(i=bloco_l; i<bloco_l + 3; i++) {
        for(j=bloco_c; j<bloco_c + 3; j++) {
			if ( numeros[matriz[i][j] - 1] == 1) {
				printf("erro no bloco %d no numero %d \n", numBloco + 1 , matriz [i][j]);
				return NULL;
			} else {
            	numeros[matriz[i][j] - 1] = 1;
			}
        }
	}
	return NULL;
}

void * verificaColuna (void *colunaAp) {
	int numeros[9] = {0};
	int i , coluna = *(int *)colunaAp;
	for (i = 0 ; i < 9 ; i ++) {
		if ( numeros[ matriz [i][coluna] - 1] == 1) {
			//errado
			printf("erro na coluna %d no numero %d \n", coluna + 1 , matriz [i][coluna]);
			return NULL;
		} else {
			numeros[ matriz [i][coluna] - 1] = 1; 
		}	
	}
	return NULL;
}

int main () {
	pthread_t thr[N_THR];
	int i,j, *p_id;
  	
	for (i = 0; i < 9; i++)
		for ( j = 0; j < 9 ; j++)
			scanf("%d", &matriz[i][j]);

	for (i = 0; i < N_THR; i++) {
	    p_id = (int*) malloc(sizeof(int));
	    *p_id = i;
	    pthread_create(&thr[i], NULL, verificaColuna, (void*) p_id);
	    pthread_create(&thr[i], NULL, verificaLinha, (void*) p_id);
	    pthread_create(&thr[i], NULL, verificaBloco, (void*) p_id);
	}
	
	for (i = 0; i < N_THR; i++) 
	    pthread_join(thr[i], NULL); 

//	printf("Matriz correta\n");
	return 0;
}
