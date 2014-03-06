/* Ulisses Malta Santos  -  RA: 140958  -  MC 202 Turma C */

#include <stdio.h>

#define TAM_TABULEIRO 9

void inicializaTabuleiro();
int sudoku(int linha,int coluna);
int verificaNumeroPosicao(int linha,int coluna,int valor);
void imprime();

int tabuleiro[TAM_TABULEIRO][TAM_TABULEIRO];

int main() {
   inicializaTabuleiro();
   int casasOcupadas, linha, coluna, valor, i;
   scanf("%d", &casasOcupadas);
   for(i = 0;i < casasOcupadas;i++) {
      scanf("%d %d %d", &linha,&coluna,&valor);
      tabuleiro[linha - 1][coluna - 1] = valor;
   }
   if(sudoku(0,0) == 1)
     imprime();
   else
      printf("Sem solucao\n");
   return(0);
}

/* Fun��o que inicializa a matriz 9x9 que armazena o jogo de Sudoku com zeros */
void inicializaTabuleiro() {
   int i, j;
   for(i = 0;i < TAM_TABULEIRO;i++)
      for(j = 0;j < TAM_TABULEIRO;j++)
         tabuleiro[i][j] = 0;
}

/* Fun��o que retorna 1 se sudoku analisado tiver solu��o ou 0 caso contr�rio
   Fun��o respons�vel pelo backtracking  */
int sudoku(int linha,int coluna) {
   int flag = 0;
   if(linha == TAM_TABULEIRO) /* Caso base | Se a linha atingiu o valor do tamanho do tabuleiro, existe solu��o */
      return(1);
   else {
      int i = 1,aux;
      while(i <= TAM_TABULEIRO && flag == 0) {
		 /* Se a fun��o verificaNumeroPosicao retorna 999, significa que i � um candidato para a resposta final */
         if(verificaNumeroPosicao(linha,coluna,i) == 999) { 
            aux = tabuleiro[linha][coluna]; /* Salvando o valor que est� no tabuleiro para fazer o backtracking */
            tabuleiro[linha][coluna] = i; /* O tabuleiro recebe um poss�vel valor */
            if(coluna < (TAM_TABULEIRO - 1))
               flag = sudoku(linha,coluna + 1); /* Avan�o a coluna */
            else
               flag = sudoku(linha + 1,0); /* Avan�o a linha e zero a coluna */
            if(flag == 0) /* Se flag for igual a 1, a resposta j� foi encontrada e o retrocesso pode parar */
               tabuleiro[linha][coluna] = aux;
         }
         i++;
      }
   }
   return(flag);
}

/* Fun��o que verifica se o n�mero passado como par�metro j� est� presente na linha, coluna ou quadrante 3x3 indicados
   (retorna 0 se o n�mero j� existir ou 1 se o n�mero n�o existir ou estiver na posi��o (linha,coluna) */
int verificaNumeroPosicao(int linha,int coluna,int valor) {
   int i, j;
   if (tabuleiro[linha][coluna] == valor) /* Verifica se o valor recebido � igual ao n�mero na posi��o (linha,coluna) */
      return(999);
   if (tabuleiro[linha][coluna] != 0) /* Verifica se o n�mero localizado na posi��o (linha,coluna) da matriz � zero e pode ser alterado ou se � outro n�mero qualquer */
      return(0);
   for (i = (linha/3)*3;i < (((linha/3) + 1) * 3);i++) /* Percorre o quadrante 3x3 onde o n�mero onde a linha e a coluna recebidas com par�metro est�o localizadas */
      for (j = (coluna/3)*3;j < (((coluna/3) + 1) * 3);j++)
         if(tabuleiro[i][j] == valor)
            return(0);
   for(i = 0;i < TAM_TABULEIRO;i++) /* Percorre a linha e a coluna indicadas */
      if(tabuleiro[i][coluna] == valor || tabuleiro[linha][i] == valor)
         return(0);
   return(999);
}

/* Funcao que imprime a matriz 9x9 que armazena o jogo de Sudoku
   (opcionalmente a matriz pode ser global e nao passada por par�metro)*/
void imprime(){
	int i, j;
	for(i = 0;i < TAM_TABULEIRO;i++){
		if(i%3 == 0)
			printf("--------------------------\n");
		for(j = 0;j < TAM_TABULEIRO;j++){
			if(j%3 == 0)
				printf(" |");
			printf(" %d", tabuleiro[i][j]);
		}
		printf(" |\n");
	}
	printf("--------------------------\n");
}
