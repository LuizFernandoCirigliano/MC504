#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int elementos;
	int dicas[9];
} Dica; 

typedef struct {
	int linha, coluna, bloco;
} Casa;

Dica final[9][9];
int tabuleiro[9][9];

void *thr_elemento (void *v) {
	Casa casa = *(Casa*)v;
	int numeros[9], i, j;
	
	memset(numeros, 0, sizeof(int)*9);
		
	for(i = 0; i < 9; i++)	{
		if (tabuleiro[i][casa.coluna] != 0)
			numeros[tabuleiro[i][casa.coluna]-1] = 1;
		if (tabuleiro[casa.coluna][i] != 0)
			numeros[tabuleiro[casa.coluna][i]-1] = 1;
	}
	
	
	for (i = (casa.linha/3)*3;i < (((casa.linha/3) + 1) * 3);i++)
      for (j = (casa.coluna/3)*3;j < (((casa.coluna/3) + 1) * 3);j++)
      	if(tabuleiro[i][j] != 0)
      		numeros[tabuleiro[i][j]-1] = 1;

    final[casa.linha][casa.coluna].elementos = 0;

    for (i = 0; i < 9; i++) {
    	if(!numeros[i]) {
    		final[casa.linha][casa.coluna].elementos++;
    		final[casa.linha][casa.coluna].dicas[final[casa.linha][casa.coluna].elementos] = i+1;
    	}
    }
	
	return NULL;
}

void leitura() {
	int i, j, n;
	for(i = 0; i < 9; i++) {
	 for(j = 0; j < 9; j++) {
	 	scanf("%c", &n);
	 	if(n > 48 && n <= 57)
		 	tabuleiro[i][j] = n - 48;
	 }
	}
	 
}



int main() {
	memset(tabuleiro, 0, sizeof(int) * 81);
	leitura();
	

	return 0;
}
