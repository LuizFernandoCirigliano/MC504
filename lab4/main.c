#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>
#define NUMERO_THREADS 9
/*Funcao que imprime a matriz 9x9 que armazena o jogo de Sudoku (opcionalmente a matriz pode ser global e nao passada por parametro)*/
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
	int i,j = 0,k,x,y,stop = 0;
	int *retorno = (int *) malloc (sizeof (int));
    int *resultado = (int *) malloc (sizeof(int));
	short **matCopy[9], **matriz;
	int possivel[10];
    
    pthread_t thr[NUMERO_THREADS];
    
	matriz = (short **) mat;
    
	if(completo(matriz)) {
		imprime(matriz);
		*retorno = 1;
		return (void *)retorno;
	}
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
		if(possivel[k]) {
            matCopy[k-1] = (short **)malloc(sizeof(short *) * 9);
			for (x = 0; x < 9; x++) {
				matCopy[k-1][x] = (short *)malloc(sizeof(short)* 9);
			}
			
			for (x = 0; x < 9 ; x++)
				for (y = 0 ; y < 9 ; y ++)
					matCopy[k-1][x][y] = matriz[x][y];
            
			matCopy[k-1][i][j] = k;
//            pthread_create(&thr[k-1], NULL, sudoku, (void *)matCopy[k-1]);
            
            
			if(* (int *) sudoku(matCopy[k - 1])) {
				*retorno = 1;
				return (void *)retorno;
			}
			matCopy[k-1][i][j] = 0;
		}
	}
    
//	for (i = 0; i < 9 ; i++)
//		if (matCopy[i-1]) {
//			pthread_join (thr[i], (void *)resultado);
//		}
	
	*retorno = 0;
	return (void *)retorno;
}

int main(){
	short **mat;
	int n,X,Y,V;
	int i;
	int *resultado = (int *) malloc (sizeof(int));
	scanf("%d",  &n);
    
	pthread_t thr[NUMERO_THREADS];
    
	mat = (short **)malloc(sizeof(short *) * 9);
	for (i = 0; i < 9; i++) {
		mat[i] = (short *)malloc(sizeof(short)* 9);
	}
	while(n--) {
		scanf("%d %d %d",&X,&Y,&V);
		mat[X-1][Y-1] = V;
	}
	pthread_create(&thr[0], NULL, sudoku, (void *)mat);
	pthread_join (thr[0], (void *)resultado);
	if(!*(int *)resultado){
		printf("Sem solucao\n");
	}
	return 0;
}