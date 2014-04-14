/*
 * 
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile int esperando = 0; /* Pessoas esperando o onibus */

sem_t sem_onibus, sem_embarcando;
pthread_mutex_t lock_onibus, lock_passageiros;

//numero maximo de pessoas esperando na plataforma
#define MAX_ESPERANDO 10
#define CAPACIDADE_ONIBUS 50

//quanto maior esse valor , menos onibus passam
#define ESCASSEZ_DE_ONIBUS 2

void vaiEmbora (int i) {
	printf("passageiro %d vai embora \n" , i);
	sleep (1);
}

void embarcar (int i) {
	printf("passageiro %d embarcou \n" , i);
	
	esperando--;
	
	sleep (1);	
}
void *onibus (void* v) {
	int id = (int) v;

	int passageiros_no_onibus = 0;

	printf("onibus %d chegou\n", id);
	

	pthread_mutex_lock(&lock_onibus);
	printf("Comecando embaque no onibus %d\n", id);
	
	while (esperando && passageiros_no_onibus < CAPACIDADE_ONIBUS ) {
		//manda o proximo embarcar
		sem_post (&sem_onibus);

		//espera fim do embarque
		sem_wait (&sem_embarcando);
		passageiros_no_onibus++;
	}

	printf("fim de embarque no onibus %d\n", id );
	pthread_mutex_unlock(&lock_onibus);

  return NULL;
}

void *passageiro (void *v) {
	int id = (int)v;
	
	printf("passageiro %d chegou\n", id);

	pthread_mutex_lock(&lock_passageiros);
	//Verifica se plataforma nao esta lotada
	if (esperando >= MAX_ESPERANDO)  {
		pthread_mutex_unlock (&lock_passageiros);
		vaiEmbora(id);
		return NULL;
	} else {
		esperando++;
	}
  	pthread_mutex_unlock(&lock_passageiros);

	sem_wait(&sem_onibus);
	embarcar(id);
	sem_post(&sem_embarcando);
	
	return NULL;
}

int main() {
  pthread_t thr;
  int numOnibus = 0, numPassageiros = 0;

  if (pthread_mutex_init(&lock_passageiros, NULL) != 0 || pthread_mutex_init(&lock_onibus, NULL) != 0) {
        printf("\n mutex falhou\n");
        return 1;
  }

  sem_init(&sem_onibus, 0, 0);
  sem_init(&sem_embarcando, 0, 0);

  while (1) {
  	sleep (1);
    if (random() % (MAX_ESPERANDO * ESCASSEZ_DE_ONIBUS) == 1) 
      pthread_create(&thr, NULL, onibus, (void*) numOnibus++);
    else
      pthread_create(&thr, NULL, passageiro, (void*) numPassageiros++);
  }

  pthread_exit(NULL);
}
