#include<stdio.h>

/*Funcao que imprime a matriz 9x9 que armazena o jogo de Sudoku (opcionalmente a matriz pode ser global e nao passada por parametro)*/
void imprime(int mat[][9]){
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

/*void imprime(int mat[][9]){
 int i, j;
 for(i=0; i<9; i++){
	 for(j=0; j<9; j++){
		 printf("%d", mat[i][j]);
		 }
	printf("\n");
 }
 return;
}*/

void checa(int mat[][9],int *possivel,int linha, int coluna) {
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

int completo (int mat[][9]){
	int i,j;
	for(i=0;i<9;i++) {
		for(j=0;j<9;j++) {
			if(mat[i][j] == 0)
				return 0;
		}
	}
	return 1;
}


int sudoku(int mat[][9]){
	int i,j,k,stop = 0;
	int possivel[10];
	if(completo(mat)) {
		imprime(mat);
		return 1;
	}
	for(i=0;i<9 && !stop;i++) {
		for(j=0;j<9 && !stop;j++) {
			if(mat[i][j] == 0)
				stop = 1;
		}
	}
	if(i>0) i--; if(j>0) j--;
	checa(mat,possivel, i, j);
	for(k=1;k<10;k++) {
		if(possivel[k]) {
			mat[i][j] = k;
			if(sudoku(mat)) return 1;
			mat[i][j] = 0;
		}
	}
	return 0;
}

int main(){
	int mat[9][9] = {{0}};
	int i,j,c,n;
	scanf("%d\n", &n);
	while(n--) {
	for(i=0;i<9;i++) {
		for(j=0;j<9;j++) {
			scanf("%c", &c);
			mat[i][j] = c - '0';
		}
		scanf("%c", &c);
	}
	if(!sudoku(mat)){
		printf("Sem solucao\n");
	}
	}
	return 0;
}