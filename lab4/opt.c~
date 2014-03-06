#include<stdio.h>

typedef struct Tpossible {
	int linha;
	int coluna;
	int count;
} possible;

int mat[9][9][10];
possible proximo[81];
/*Funcao que imprime a matriz 9x9 que armazena o jogo de Sudoku (opcionalmente a matriz pode ser global e nao passada por parametro)*/
void imprime(){
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

void checa(int *possivel,int linha, int coluna) {
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

int completo (){
	int i,j;
	for(i=0;i<9;i++) {
		for(j=0;j<9;j++) {
			if(mat[i][j] == 0)
				return 0;
		}
	}
	return 1;
}


int sudoku(){
	int i,j,k,stop = 0;
	int possivel[10];
	if(completo()) {
		imprime();
		return 1;
	}
	for(i=0;i<9 && !stop;i++) {
		for(j=0;j<9 && !stop;j++) {
			if(mat[i][j] == 0)
				stop = 1;
		}
	}
	if(i>0) i--; if(j>0) j--;
	checa(possivel, i, j);
	for(k=1;k<10;k++) {
		if(possivel[k]) {
			mat[i][j] = k;
			if(sudoku()) return 1;
			mat[i][j] = 0;
		}
	}
	return 0;
}

void ordena(int p) {
	possible temp = proximo[p];
	while(p>0 && temp.count < proximo[p-1].count) {
		proximo[p] = proximo[p-1];
		p--;
	}
	proximo[p] = temp;
}

int main(){
	int n,X,Y,V;
	int i,j,k;
	for(i=0;i<9;i++)
		for(j=0;j<9;j++) {
			proximo[i*9+j].linha = i;
			proximo[i*9+j].coluna = j;
			proximo[i*9+j].count = 9;
			for(k=1;k<=9;k++)
				mat[i][j][k] = 1;
		}		
	scanf("%d",  &n);
	while(n--) {
		scanf("%d %d %d",&X,&Y,&V);
		mat[X-1][Y-1][0] = V;
		proximo[(x-1)*9+(y-1)] = 0;
		ordena((x-1)*9+(y-1));
	}
	if(!sudoku()) {
		printf("Sem solucao\n");
	}
	return 0;
}