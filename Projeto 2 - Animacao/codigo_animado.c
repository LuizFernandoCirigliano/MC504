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

sem_t sem_onibus, sem_embarcando;
pthread_mutex_t lock;

WINDOW *history, *bus, *line;

//numero maximo de pessoas esperando na plataforma
#define MAX_ESPERANDO 10

//quanto maior esse valor , menos onibus passam
#define ESCASSEZ_DE_ONIBUS 2

#define PROPORCAO_PASSAGEIRO_ONIBUS 2

void clearHistory() {
  int x, y;
	getyx(history, y, x);
	if(y == 14)
	  wclear(history);
}

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

void  busArrive() {
  wclear(bus);
  wprintw(bus, "______________________\n|,----.,----.,----.,--.\\\n||    ||    ||    ||   \\\\\n|`----'`----'|----||----\\`.\n[            |   -||- __|(|\n[  ,--.      |____||.--.  |\n=-(( `))-----------(( `))==\n   `--'             `--'");  
  wrefresh(bus);
}

void busDepart() {
  wclear(bus);
  wrefresh(bus);
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
  refreshLine();
	
	sleep (1);	
}
void *onibus (void* v) {
	int id = (int) v;
	int i = 0, passageiros_para_embarcar;
	
	clearHistory();
	wprintw(history, "onibus %d chegou\n", id);
  wrefresh(history);
  
  busArrive();

	pthread_mutex_lock(&lock);

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
	
	clearHistory();
	wprintw(history, "passageiro %d chegou\n", id);
	wrefresh(history);

	pthread_mutex_lock(&lock);
	
	//Verifica se plataforma nao esta lotada
	if (esperando >= MAX_ESPERANDO)  {
		pthread_mutex_unlock (&lock);
		vaiEmbora(id);
		return NULL;
	} else {
		esperando++;
		refreshLine();
		pthread_mutex_unlock(&lock);
	}
  	

	sem_wait(&sem_onibus);
	embarcar(id);
	sem_post(&sem_embarcando);
	
	return NULL;
}

int main() {
  initscr();
  
  history = newwin(15, COLS, LINES-15, 0);
  wrefresh(history);
  
  line = newwin((LINES - 15)/2, COLS, 0, 0);
  wrefresh(line);
  
  bus = newwin((LINES - 15)/2, COLS, (LINES - 15)/2, 0);
  wrefresh(bus);
  
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
