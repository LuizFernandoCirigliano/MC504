/*
 * 
 */
#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile int esperando = 0; /* Pessoas esperando o onibus */
volatile int embarcaram = 0, busPassed = 0;
sem_t sem_onibus, sem_embarcando;
pthread_mutex_t lock;

WINDOW *history, *bus, *line, *stat;

//numero maximo de pessoas esperando na plataforma
#define MAX_ESPERANDO 10

//quanto maior esse valor , menos onibus passam
#define ESCASSEZ_DE_ONIBUS 2

#define PROPORCAO_PASSAGEIRO_ONIBUS 2

/* A janela do historico de ações pode conter no máximo 15 linhas */
void clearHistory() {
  int x, y;
	getyx(history, y, x);
	if(y == 14)
	  wclear(history);
}

/* Redesenha a fila de espera */
void	refreshLine() {
  int i;
  wclear(line);
  for(i = 0; i < esperando; i++)
    wprintw(line, " o  ");
  wprintw(line, "\n");
  for(i = 0; i < esperando; i++)
    wprintw(line, "/|\\ ");
  wprintw(line, "\n");
  for(i = 0; i < esperando; i++)
    wprintw(line, "/ \\ ");
  wrefresh(line);
}

/* Desnha o ônibus assim  que ele chega */
void  busArrive() {
  wclear(bus);
  wprintw(bus, "______________________\n|,----.,----.,----.,--.\\\n||    ||    ||    ||   \\\\\n|`----'`----'|----||----\\`.\n[            |   -||- __|(|\n[  ,--.      |____||.--.  |\n=-(( `))-----------(( `))==\n   `--'             `--'");  
  wrefresh(bus);
}

/* Quando o ônibus parte, apaga a janela do ônibus */
void busDepart() {
  wclear(bus);
  wrefresh(bus);
}

/* Atualiza a janela onde são mostradas as estatísticas */
void refreshStat() {
  int x, y;
  wclear(stat);
  wprintw(stat, "Passageiros\nesperando:\n");
  getyx(stat, y, x);
  attron(A_BOLD);
  mvwprintw(stat, y, 15/2, "%d", esperando);  
  attroff(A_BOLD);

  wprintw(stat, "\nTotal\npassageiros\nembarcaram:\n");
  getyx(stat, y, x);
  attron(A_BOLD);
  mvwprintw(stat, y, 15/2, "%d", embarcaram);  
  attroff(A_BOLD);
  
  wprintw(stat, "\nTotal\nônibus\npassaram:\n");
  getyx(stat, y, x);
  attron(A_BOLD);
  mvwprintw(stat, y, 15/2, "%d", busPassed);  
  attroff(A_BOLD);
  
  wrefresh(stat); 
}

void vaiEmbora (int i) {
	clearHistory();
	wprintw(history, "passageiro %d vai embora \n" , i);
	wrefresh(history);
	sleep (1);
}

void embarcar (int i) {
  clearHistory();
	wprintw(history, "passageiro %d embarcou \n" , i);
	wrefresh(history);
	
  esperando--;
  embarcaram++;
  refreshLine();
  refreshStat();
	
	sleep (1);	
}

void *onibus (void* v) {
	int id = (int) v;
	int i = 0, passageiros_para_embarcar;
	
	pthread_mutex_lock(&lock);
	
	clearHistory();
	wprintw(history, "onibus %d chegou\n", id);
  wrefresh(history);
  
  busPassed++;
  refreshStat();
  busArrive();

	clearHistory();
	wprintw(history, "Comecando embaque no onibus %d\n", id);
	wrefresh(history);
	
	passageiros_para_embarcar = esperando;
	for (i = 0 ; i  < passageiros_para_embarcar; i ++ ) {
		//manda o proximo embarcar
		sem_post (&sem_onibus);

		//espera fim do embarque
		sem_wait (&sem_embarcando);
	}

	clearHistory();
	wprintw(history, "fim de embarque no onibus %d\n", id );
	wrefresh(history);
	busDepart();
	
	pthread_mutex_unlock(&lock);

  return NULL;
}

void *passageiro (void *v) {
	int id = (int)v;
	
	pthread_mutex_lock(&lock);
	
	clearHistory();
	wprintw(history, "passageiro %d chegou\n", id);
	wrefresh(history);
	
	//Verifica se plataforma nao esta lotada
	if (esperando >= MAX_ESPERANDO)  {
		pthread_mutex_unlock (&lock);
		vaiEmbora(id);
		return NULL;
	} else {
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

int main() {
  /* Inicializa o ncurses */
  initscr();
  
  /* Inicializa as janelas da animação */
  history = newwin(15, COLS-15, LINES-15, 0);
  wrefresh(history);
  
  line = newwin((LINES - 15)/2, COLS-15, 0, 0);
  wrefresh(line);
  
  bus = newwin((LINES - 15)/2, COLS-15, (LINES - 15)/2, 0);
  wrefresh(bus);
  
  stat = newwin(LINES, 15, 0, COLS-15);
  wrefresh(stat);
  /* Fim da inicialização */
  
  pthread_t thr;
  int numOnibus = 0, numPassageiros = 0;

  if (pthread_mutex_init(&lock, NULL) != 0) {
        wprintw(history, "\n mutex falhou\n");
        wrefresh(history);
        return 1;
  }

  sem_init(&sem_onibus, 0, 0);
  sem_init(&sem_embarcando, 0, 0);
  
  while (1) {
  	sleep (1);
    if (random() % (MAX_ESPERANDO * PROPORCAO_PASSAGEIRO_ONIBUS) == 1) 
      pthread_create(&thr, NULL, onibus, (void*) numOnibus++);
    else
      pthread_create(&thr, NULL, passageiro, (void*) numPassageiros++);
  }
  endwin();
  pthread_exit(NULL);
}
