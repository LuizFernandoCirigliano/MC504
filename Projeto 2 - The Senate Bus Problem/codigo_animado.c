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

/* Quanto maior esse valor, menos ônibus passam. */
#define ESCASSEZ_DE_ONIBUS 2

#define PROPORCAO_PASSAGEIRO_ONIBUS 2

volatile int esperando = 0;			/* Contador que marca o número de pessoas esperando o ônibus. */
volatile int embarcaram = 0;		/* Contador que marca o número de pessoas que embarcaram no ônibus. */
volatile int busPassed = 0;			/* Contador que marca o número de ônibus que passaram pelo ponto. */

sem_t sem_onibus, sem_embarcando;
pthread_mutex_t lock;

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
  mvwin(bus, (int)LINES/4 , 0) ;
  wclear(bus);
  wprintw(bus, "______________________\n|,----.,----.,----.,--.\\\n||    ||    ||    ||   \\\\\n|`----'`----'|----||----\\`.\n[            |   -||- __|(|\n[  ,--.      |____||.--.  |\n=-(( `))-----------(( `))==\n   `--'             `--'");  
  wrefresh(bus);
}

/* Quando o ônibus parte, apaga a janela do ônibus. */
void busDepart() {
	int i =0 ;
	
	while(mvwin(bus, (int)LINES/4 , i++) != -1) {
	
		wrefresh(bus);

		usleep(50000);

	}
	wclear(bus);
	wrefresh(bus);
}

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
	int i = 0, passageiros_para_embarcar;
	
	pthread_mutex_lock(&lock);
	
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
	
	passageiros_para_embarcar = esperando;
	
	for (i = 0 ; i  < passageiros_para_embarcar; i ++) {
		/* Manda o próximo passageiro embarcar. */
		sem_post (&sem_onibus);

		/* Espera o fim do embarque. */
		sem_wait (&sem_embarcando);
	}

	clearHistory();

	wprintw(history, " fim do embarque no ônibus %d.\n", id);
	wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
	wrefresh(history);
	
	busDepart();
	
	pthread_mutex_unlock(&lock);

	return NULL;
}

void* passageiro(void* v) {
	int id = (int)v;
	
	pthread_mutex_lock(&lock);
	
	usleep(100000);
	
	clearHistory();
	
	wprintw(history, " passageiro %d chegou.\n", id);
	wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
	wrefresh(history);
	
	/* Verifica se a plataforma está lotada. */
	if (esperando >= MAX_ESPERANDO)  {
		/* Se a plataforma já está lotada, o novo passageiro vai embora. */
		pthread_mutex_unlock (&lock);
		vaiEmbora(id);
		return NULL;
	} 
	else {
		/* Caso contrário, ele fica esperando o ônibus. */
		esperando++;
		refreshLine();
		refreshStat();
		pthread_mutex_unlock(&lock);
	}  	

	sem_wait(&sem_onibus);
	embarcar(id);
	sem_post(&sem_embarcando);
	
	return NULL;
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
	
	bus = newwin((int)LINES/2, COLS/4, (int)LINES/4, 0);
	wrefresh(bus);
	
	history = newwin((int)LINES/4, (int)COLS/2, (int)3*LINES/4, 0);
	wprintw(history, "\n");
	wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
	wrefresh(history);	
  
	stat = newwin((int)LINES/4, (int)COLS/2, (int)3*LINES/4, (int)COLS/2);
	wborder(stat, '|', '|', '-', '-', '-', '+', '-', '+');
	wrefresh(stat);
	/* Fim da inicialização; */ 
  
	pthread_t thr;
	int numOnibus = 0, numPassageiros = 0;

	if (pthread_mutex_init(&lock, NULL) != 0) {
		endwin();
		printf("Mutex falhou!!!\n");		
        exit(1);
	}

	sem_init(&sem_onibus, 0, 0);
	sem_init(&sem_embarcando, 0, 0);
  
	while(1) {
		if(random() % (MAX_ESPERANDO * PROPORCAO_PASSAGEIRO_ONIBUS) == 1) 
			pthread_create(&thr, NULL, onibus, (void*) numOnibus++);
		else
			pthread_create(&thr, NULL, passageiro, (void*) numPassageiros++);
		usleep(1500000);
	}
	
	endwin();
	
	pthread_exit(NULL);
}
