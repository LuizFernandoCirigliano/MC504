/*
*	Grupo:
*		Ulisses Malta Santos					140958
*		Luiz Fernando Cirigliano Villela		136734
*		José Ernesto Stelzer Monar				139553
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>

/* Número máximo de pessoas esperando pelo ônibus na plataforma. */
#define MAX_ESPERANDO 10


#define CAPACIDADE_ONIBUS 15
#define PROPORCAO_PASSAGEIRO_ONIBUS 2
#define THREADSTACK  65536
#define MAX_THREADS  10000

volatile int esperando = 0;			/* Contador que marca o número de pessoas esperando o ônibus. */
volatile int embarcaram = 0;		/* Contador que marca o número de pessoas que embarcaram no ônibus. */
volatile int busPassed = 0;			/* Contador que marca o número de ônibus que passaram pelo ponto. */

sem_t sem_onibus, sem_embarcando;
pthread_mutex_t lock_onibus, lock_passageiros;


WINDOW *history, *bus, *ponto, *stat;

/* Redesenha a fila de espera */
void refreshLine() {
	int i;
	wclear(ponto);
	for(i = 0; i < esperando; i++)
		wprintw(ponto, " o  ");
	wprintw(ponto, "\n");
	for(i = 0; i < esperando; i++)
		wprintw(ponto, "/|\\ ");
	wprintw(ponto, "\n");
	for(i = 0; i < esperando; i++)
		wprintw(ponto, "/ \\ ");
	wrefresh(ponto);
}

/* Desnha o ônibus assim  que ele chega */
void busArrive() {
  wclear(bus);
  
  
  
  wprintw(bus, "______________________\n|,----.,----.,----.,--.\\\n||    ||    ||    ||   \\\\\n|`----'`----'|----||----\\`.\n[            |   -||- __|(|\n[  ,--.      |____||.--.  |\n=-(( `))-----------(( `))==\n   `--'             `--'");  
  wrefresh(bus);
}

/* Quando o ônibus parte, apaga a janela do ônibus. */
void busDepart() {
	int i, j;

	char str[29];
	char str1[29] = "______________________";
	char str2[29] = "|,----.,----.,----.,--.\\";
	char str3[29] = "||    ||    ||    ||   \\\\";
	char str4[29] = "|`----'`----'|----||----\\`.";
	char str5[29] = "[            |   -||- __|(|";
	char str6[29] = "[  ,--.      |____||.--.  |";
	char str7[29] = "=-(( `))-----------(( `))==";
	char str8[29] = "   `--'             `--'";

	for(i = 0; i <= COLS; i++) {
		if(COLS - i > 29)
			j = 29;
		else
			j = COLS - i
			;
		wclear(bus);
		memset (str,'\0', 29);
		strncpy(str, str1, j);
		mvwaddstr(bus, 0, i, str);
		memset (str,'\0', 29);
		strncpy(str, str2, j);
		mvwaddstr(bus, 1, i, str);
		memset (str,'\0', 29);
		strncpy(str, str3, j);
		mvwaddstr(bus, 2, i, str);
		memset (str,'\0', 29);
		strncpy(str, str4, j);
		mvwaddstr(bus, 3, i, str);
		memset (str,'\0', 29);
		strncpy(str, str5, j);
		mvwaddstr(bus, 4, i, str);
		memset (str,'\0', 29);
		strncpy(str, str6, j);
		mvwaddstr(bus, 5, i, str);
		memset (str,'\0', 29);
		strncpy(str, str7, j);
		mvwaddstr(bus, 6, i, str);
		memset (str,'\0', 29);
		strncpy(str, str8, j);
		mvwaddstr(bus, 7, i, str);

		usleep(50000);
		wrefresh(bus);
	}

	wclear(bus);
}

/* Atualiza a janela onde são mostradas as estatísticas */
void refreshStat() {
	wclear(stat);  

	attron(A_BOLD);

	wprintw(stat, "\n passageiros esperando: %d\n", esperando);
	wprintw(stat, " passageiros que embarcaram: %d\n", embarcaram);
	wprintw(stat, " ônibus que passaram: %d\n", busPassed);
  
	wborder(stat, '|', '|', '-', '-', '-', '+', '-', '+');  

	wrefresh(stat);

	attroff(A_BOLD);
}

/* Atualiza a janela do histórico. */
void clearHistory() {
	int x, y;
	getyx(history, y, x);
	x++;
	if(y == ((int)LINES/4)-2) {
		wclear(history);
		wprintw(history, "\n");
		wrefresh(history);
	}
}

void vaiEmbora(int i) {
	clearHistory();

	wprintw(history, " passageiro %d vai embora.\n", i);
	wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
	wrefresh(history);

	usleep(1000000);
}

void embarcar(int i) {
	clearHistory();
	wprintw(history, " passageiro %d embarcou.\n", i);
	wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
	wrefresh(history);

	esperando--;
	embarcaram++;
	refreshLine();
	refreshStat();

	usleep(1000000);
}

void* onibus(void* v) {
	int id = (int)v;
	int passageiros_no_onibus = 0;

	pthread_mutex_lock(&lock_onibus);

	clearHistory();

	wprintw(history, " ônibus %d chegou!!!\n", id);

	wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
	wrefresh(history);
  
	busPassed++;
	refreshStat();
	busArrive();

	clearHistory();


	wprintw(history, " começando embarque no ônibus %d. \n", id);

	wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
	wrefresh(history);



	while (esperando > 0 && passageiros_no_onibus < CAPACIDADE_ONIBUS) {
		/* Manda o próximo passageiro embarcar. */
		sem_post (&sem_onibus);

		/* Espera o fim do embarque. */
		sem_wait (&sem_embarcando);
		passageiros_no_onibus++;
	}

	clearHistory();

	wprintw(history, " fim do embarque no ônibus %d.\n", id);
	wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
	wrefresh(history);

	busDepart();

	pthread_mutex_unlock(&lock_onibus);

	pthread_exit(NULL);
}

void* passageiro(void* v) {
	int id = (int)v;

	pthread_mutex_lock(&lock_passageiros);




	clearHistory();
	wprintw(history, " passageiro %d chegou.\n", id);
	wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
	wrefresh(history);

	usleep(100000);
	/* Verifica se a plataforma está lotada. */
	if (esperando >= MAX_ESPERANDO)  {
		/* Se a plataforma já está lotada, o novo passageiro vai embora. */
		vaiEmbora(id);

		pthread_mutex_unlock (&lock_passageiros);

		pthread_exit(NULL);
 	} 
	else {
		/* Caso contrário, ele fica esperando o ônibus. */
		esperando++;
		refreshLine();
		refreshStat();
		pthread_mutex_unlock(&lock_passageiros);
	}  	

	sem_wait(&sem_onibus);
	embarcar(id);
	sem_post(&sem_embarcando);

	pthread_exit(NULL);
}

void terminal_mudou_tamanho(int sinal) {

}

int main() {	
	/* Instala um novo tratador para o sinal recebido quando a janela do terminal muda de tamanho. */
	signal(SIGWINCH, terminal_mudou_tamanho);
  
	/* Inicializa a biblioteca ncurses. */
	initscr();

	if(LINES < 20 || COLS < 40) {
		endwin();
		printf("Redimensione o terminal. Ele deve ter pelo menos 20 linhas e 40 colunas.\n");
		exit(1);
	}

	/* Inicializa as janelas da animação. */
	ponto = newwin((int)LINES/4, COLS, 0, 0);	
	wrefresh(ponto);

	bus = newwin((int)LINES/2, COLS, (int)LINES/4, 0);
	wrefresh(bus);

	history = newwin((int)LINES/4, (int)COLS/2, (int)3*LINES/4, 0);
	wprintw(history, "\n");
	wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
	wrefresh(history);	
  
	stat = newwin((int)LINES/4, (int)COLS/2, (int)3*LINES/4, (int)COLS/2);
	wborder(stat, '|', '|', '-', '-', '-', '+', '-', '+');
	wrefresh(stat);
	/* Fim da inicialização; */ 
  
	pthread_t thr[MAX_THREADS];
	int i,numOnibus = 0, numPassageiros = 0;

    pthread_attr_t  attrs;

    pthread_attr_init(&attrs);
    pthread_attr_setstacksize(&attrs, THREADSTACK);

	if (pthread_mutex_init(&lock_passageiros, NULL) != 0 || pthread_mutex_init(&lock_onibus, NULL) != 0) {
        printf("\n mutex falhou\n");
        return 1;
  }

	sem_init(&sem_onibus, 0, 0);
	sem_init(&sem_embarcando, 0, 0);
  
	for (i = 0 ; i < MAX_THREADS; i++) {
		if(random() % (MAX_ESPERANDO * PROPORCAO_PASSAGEIRO_ONIBUS) == 1) 
			pthread_create(&thr[i], &attrs, onibus, (void*) numOnibus++);
		else
			pthread_create(&thr[i], &attrs, passageiro, (void*) numPassageiros++);
		usleep((random() % 3) *1500000);
	}

	endwin();

	return 0;
}
