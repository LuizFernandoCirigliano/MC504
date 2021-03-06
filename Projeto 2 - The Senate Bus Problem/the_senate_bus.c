/*
*   ---- The Senate Bus Problem ----
*	Grupo:
*		Ulisses Malta Santos						140958
*		Luiz Fernando Cirigliano Villela		136734
*		José Ernesto Stelzer Monar				139553
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ncurses.h>
#include <panel.h>
#include <pthread.h>
#include <semaphore.h>

/* Tamanho da pilha de cada thread. */
#define THREADSTACK  32000

/* Tempo entre passagem de 2 onibus. */
#define INTERVALO_ONIBUS 30
/* Tempo máximo possível entre chegada de 2 pasageiros. */
#define INVERVALO_MAX_PASSAGEIROS 10

void inicializar_janelas();
void print_in_history(char*, chtype);
void print_in_stat();
void bus_arrive(int);
void bus_depart(int);
void passenger_arrive(int);
void passenger_depart(int);
void passenger_goes_away(int);
void terminal_mudou_tamanho(int);
void* passageiro(void*);
void* onibus(void*);
void* geraPassageiros(void*);
void* geraOnibus(void*);

/** Parâmetros variáveis **/

volatile int esperando = 0;			/* Contador que marca o número de pessoas esperando o ônibus. */
volatile int embarcaram = 0;			/* Contador que marca o número de pessoas que embarcaram no ônibus. */
volatile int busPassed = 0;			/* Contador que marca o número de ônibus que passaram pelo ponto. */

/* Número máximo de pessoas esperando pelo ônibus na plataforma. */
volatile int max_passageiros_ponto = 0;
/* Número máximo de pessoas que um ônibus pode transportar. */
volatile int capacidade_onibus = 0;

sem_t sem_onibus, sem_embarcando;
pthread_mutex_t lock_onibus, lock_passageiros, lock_ponto;

/* Janelas utilizadas durante o programa. */
WINDOW *ponto, *bus, *history, *stat;

PANEL	*my_panels[4];

/*Inicializa variaveis e entra num loop que gera threads de onibus ou passageiro aleatoriamente*/
int main() {
	/* Entrada dos parâmetros. */
	do {
		printf("Máximo de passageiros esperando no ponto (1 a 10): ");
		scanf("%d", &max_passageiros_ponto);
	} while(max_passageiros_ponto < 1 || max_passageiros_ponto > 10);
	
	do {
		printf("Capacidade máxima do ônibus (1 a 15): ");
		scanf("%d", &capacidade_onibus);
	} while(capacidade_onibus < 1 || capacidade_onibus > 15);
	
	/* Instala um novo tratador para o sinal recebido quando a janela do terminal muda de tamanho. */
	signal(SIGWINCH, terminal_mudou_tamanho);
  
	/* Inicializa a biblioteca ncurses. */
	initscr();
	start_color();
	cbreak();
	noecho();
	
	/* Verifica o tamanho do terminal. */
	if(LINES < 20 || COLS < 40) {
		endwin();
		printf("Redimensione o terminal. Ele deve ter pelo menos 20 linhas e 40 colunas.\n");
		exit(1);
	}

	/* Verifica se o terminal tem suporte a cores. */
	if(has_colors() == FALSE) {
		endwin();
		printf("O seu terminal não tem suporte a cores.\n");
		exit(1);
	}
	
	/* Inicialização de cores. */
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_BLUE, COLOR_BLACK);
	
	inicializar_janelas();
	
	my_panels[0] = new_panel(ponto);
	my_panels[1] = new_panel(bus);
	my_panels[2] = new_panel(history); 
	my_panels[3] = new_panel(stat); 
	
	update_panels();
	doupdate();
	
	print_in_stat();
  
	pthread_t thr[2];
	pthread_attr_t  attrs;
	
	pthread_attr_init(&attrs);
	pthread_attr_setstacksize(&attrs, THREADSTACK);
	
	/* Verifica se os locks foram inicializados corretamente. */
	if(pthread_mutex_init(&lock_passageiros, NULL) != 0 || pthread_mutex_init(&lock_onibus, NULL) != 0 ||
		pthread_mutex_init(&lock_ponto, NULL) != 0) {
		endwin();
		printf("Mutex falhou.\n");
		exit(1);
	}
	
	sem_init(&sem_onibus, 0, 0);
	sem_init(&sem_embarcando, 0, 0);
  
	pthread_create(&thr[0], &attrs, geraPassageiros,NULL);
	pthread_create(&thr[1], &attrs, geraOnibus,NULL);
		
	pthread_join (thr[0] , NULL);
	pthread_join (thr[1] , NULL);

	endwin();

	return 0;
}

void* geraPassageiros(void* v) {
	pthread_t p;
	int numPassageiros = 0;

	pthread_attr_t  attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setstacksize(&attrs, THREADSTACK);
	
	srand(time(NULL));

	while(1) {
		pthread_create(&p, &attrs, passageiro, (void*) numPassageiros++);
		sleep(rand()%INVERVALO_MAX_PASSAGEIROS);
	}

	pthread_exit(NULL);
}

void* geraOnibus(void* v) {
	pthread_t o;
	int numOnibus = 0;

	pthread_attr_t  attrs;
	pthread_attr_init(&attrs);
	pthread_attr_setstacksize(&attrs, THREADSTACK);
	
	while(1) {
		sleep(INTERVALO_ONIBUS);
		pthread_create(&o, &attrs, onibus, (void*) numOnibus++);		
	}

	pthread_exit(NULL);
}

/*Funcao que simula o comportamento dos passageiros*/
void* passageiro(void* v) {
	int id = (int)v;

	pthread_mutex_lock(&lock_passageiros);
	
	/* Verifica se a plataforma está lotada. */
	if(esperando >= max_passageiros_ponto) {
		/* Se a plataforma já está lotada, o novo passageiro vai embora. */
		passenger_goes_away(id);		
		pthread_mutex_unlock (&lock_passageiros);
		pthread_exit(NULL);
 	} 
	else {
		/* Caso contrário, ele fica esperando o ônibus. */		
		passenger_arrive(id);
		pthread_mutex_unlock(&lock_passageiros);
	}  	

	sem_wait(&sem_onibus);
	passenger_depart(id);
	sem_post(&sem_embarcando);

	pthread_exit(NULL);
}

/*Funcao que simula comportamento de onibus*/
void* onibus(void* v) {
	int id = (int)v;
	int passageiros_no_onibus = 0;

	pthread_mutex_lock(&lock_onibus);

	bus_arrive(id);

	while(esperando > 0 && passageiros_no_onibus < capacidade_onibus) {
		/* Manda o próximo passageiro embarcar. */
		sem_post (&sem_onibus);

		/* Espera o fim do embarque. */
		sem_wait (&sem_embarcando);
		passageiros_no_onibus++;
	}

	bus_depart(id);

	pthread_mutex_unlock(&lock_onibus);

	pthread_exit(NULL);
}

/* Tratador que é chamado quando o terminal é redimensionado. */

void terminal_mudou_tamanho(int sinal) {
	endwin();
   printf("Terminal mudou de tamanho.\n");
   exit(1);
}

/* Inicializa as janelas da animação. */
void inicializar_janelas() {
	ponto = newwin((int)LINES/4, COLS, 0, 0);	

	bus = newwin((int)LINES/2, COLS, (int)LINES/4, 0);

	history = newwin((int)LINES/4, (int)COLS/2, (int)3*LINES/4, 0);
	wattron(history, COLOR_PAIR(2));
	wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
	wattroff(history, COLOR_PAIR(2));
  
	stat = newwin((int)LINES/4, (int)COLS/2, (int)3*LINES/4, (int)COLS/2);
	wattron(stat, COLOR_PAIR(2));
	wborder(stat, '|', '|', '-', '-', '-', '+', '-', '+');
	wattroff(stat, COLOR_PAIR(2));
}

/* Função que imprime mensagens na janela de histórico. */
void print_in_history(char* string, chtype color) {	
	int x, y;
	static int linha = 1;
	if(history == NULL) return;
	/* Verifica se ainda há espaço na janela para imprimir a mensagem. */
	if(linha == ((int)LINES/4)-1) {
		/* Se a janela estiver cheia, é apagada para dar lugar a nova mensagem. */
		wclear(history);
		wattron(history, COLOR_PAIR(2));
		wborder(history, '|', '|', '-', '-', '+', '-', '+', '-');
		wattroff(history, COLOR_PAIR(2));
		linha = 1;
	}
	getmaxyx(history, y, x);
	y = linha;
	x = (int)(x - strlen(string))/2;
	/* Imprime a mensagem no centro da janela de acordo com a cor recebida como parâmetro. */
	wattron(history, A_BOLD);
	wattron(history, color);
	mvwprintw(history, y, x, "%s", string);	
	wattroff(history, color);
	wattroff(history, A_BOLD);
	linha++;
	refresh();
	update_panels();
	doupdate();
}

/* Função que atualiza a janela de estatísticas. */
void print_in_stat() {
	wclear(stat);	
	wattron(stat, A_BOLD);
	wattron(stat, COLOR_PAIR(2));
	mvwprintw(stat, 1, 1, "passageiros esperando: %d", esperando);
	mvwprintw(stat, 2, 1, "passageiros que embarcaram: %d", embarcaram);
	mvwprintw(stat, 3, 1, "ônibus que passaram: %d", busPassed);
	wattroff(stat, A_BOLD);	
	wborder(stat, '|', '|', '-', '-', '-', '+', '-', '+');
	wattroff(stat, COLOR_PAIR(2));
	refresh();
	update_panels();
	doupdate();
}

/* Função que imprime na tela o ônibus chegando no ponto. */
void bus_arrive(int id) {
	int i, j;	
	char* p[8];	
	char* aux;
	p[0] = "______________________";
	p[1] = "|,----.,----.,----.,--.\\";
	p[2] = "||    ||    ||    ||   \\\\";
	p[3] = "|`----'`----'|----||----\\`.";
	p[4] = "[            |   -||- __|(|";
	p[5] = "[  ,--.      |____||.--.  |";
	p[6] = "=-(( `))-----------(( `))==";
	p[7] = "   `--'             `--'";
	wattron(bus, A_BOLD);		
	wattron(bus, COLOR_PAIR(2));
	for(i = 27; i >= 0; i--) {
		wclear(bus);
		for(j = 0; j < 8; j++) {
			aux = p[j] + i;
			mvwprintw(bus, j, 0, "%s", aux);
		}
		refresh();
		update_panels();
		doupdate();
		usleep(10000);
	}
	wattroff(bus, COLOR_PAIR(2));
	wattroff(bus, A_BOLD);
	char message[29];
	sprintf(message, "ônibus %d chegou", id);
	print_in_history(message, COLOR_PAIR(3));
	
	busPassed++;
	
	print_in_stat();
}

/* Função que imprime na tela o ônibus saindo do ponto e indo embora. */
void bus_depart(int id) {
	int i, j, k;	
	char* p[8];	
	char aux[29];
	p[0] = "______________________";
	p[1] = "|,----.,----.,----.,--.\\";
	p[2] = "||    ||    ||    ||   \\\\";
	p[3] = "|`----'`----'|----||----\\`.";
	p[4] = "[            |   -||- __|(|";
	p[5] = "[  ,--.      |____||.--.  |";
	p[6] = "=-(( `))-----------(( `))==";
	p[7] = "   `--'             `--'";
	sprintf(aux, "ônibus %d saiu", id);
	print_in_history(aux, COLOR_PAIR(3));
	wattron(bus, A_BOLD);	
	wattron(bus, COLOR_PAIR(2));
	for(i = 0; i <= COLS; i++) {
		if(COLS - i > 29) j = 29;
		else j = COLS - i;
		wclear(bus);
		for(k = 0; k < 8; k++) {
			memset (aux,'\0', 29);
			strncpy(aux, p[k], j);
			mvwprintw(bus, k, i, "%s", aux);
		}
		refresh();
		update_panels();
		doupdate();
		usleep(10000);
	}
	wattroff(bus, COLOR_PAIR(2));
	wattroff(bus, A_BOLD);
}

/* Função que mostra um passageiro chegando no ponto. */
void passenger_arrive(int id) {
	pthread_mutex_lock(&lock_ponto);
	int tamanho_esperando = 4*esperando;
	int espaco_livre = (int)(COLS - tamanho_esperando)/4;
	int i, j = 4, k;
	wattron(ponto, A_BOLD);
	wattron(ponto, COLOR_PAIR(2));
	for(i = 0; i < espaco_livre; i++) {		
		for(k = 0; k < 3; k++) {
			wmove(ponto, k, tamanho_esperando);
			wclrtoeol(ponto); 
		}		
		mvwprintw(ponto, 0, COLS-j, " o  ");
		mvwprintw(ponto, 1, COLS-j, "/|\\ ");
		mvwprintw(ponto, 2, COLS-j, "/ \\ ");
		refresh();
		update_panels();
		doupdate();
		usleep(60000);
		j += 4;
	}
	wattroff(ponto, COLOR_PAIR(2));
	wattroff(ponto, A_BOLD);
	char message[29];
	sprintf(message, "passageiro %d chegou", id);
	print_in_history(message, COLOR_PAIR(2));
	
	esperando++;
	
	print_in_stat();
	pthread_mutex_unlock(&lock_ponto);
}

/* Função que mostra o passageiro embarcando no ônibus. */
void passenger_depart(int id) {
	pthread_mutex_lock(&lock_ponto);
	wattron(ponto, A_BOLD);
	wattron(ponto, COLOR_PAIR(2));
	mvwprintw(ponto, 0, 0, "    ");
	mvwprintw(ponto, 1, 0, " o  ");
	mvwprintw(ponto, 2, 0, "/|\\ ");
	refresh();
	update_panels();
	doupdate();
	usleep(200000);
	mvwprintw(ponto, 1, 0, "    ");
	mvwprintw(ponto, 2, 0, " o  ");
	refresh();
	update_panels();
	doupdate();
	usleep(200000);
	mvwprintw(ponto, 2, 0, "    ");
	refresh();
	update_panels();
	doupdate();
	usleep(200000);
	char message[29];
	sprintf(message, "passageiro %d embarcou", id);
	print_in_history(message, COLOR_PAIR(1));
	
	esperando--;
	embarcaram++;

	print_in_stat();
	usleep(200000);
	int i, j = 4, k = esperando;
	for(i = 0; i < k; i++) {
		mvwprintw(ponto, 0, j-4, " o  ");
		mvwprintw(ponto, 1, j-4, "/|\\ ");
		mvwprintw(ponto, 2, j-4, "/ \\ ");
		mvwprintw(ponto, 0, j, "    ");
		mvwprintw(ponto, 1, j, "    ");
		mvwprintw(ponto, 2, j, "    ");
		refresh();
		update_panels();
		doupdate();
		usleep(60000);
		j += 4;
	}
	wattroff(ponto, COLOR_PAIR(2));
	wattroff(ponto, A_BOLD);
	pthread_mutex_unlock(&lock_ponto);
}

/* Função que é chamada quando um passageiro chega no ponto, mas ele já está lotado
   e então vai embora. */
void passenger_goes_away(int id) {
	pthread_mutex_lock(&lock_ponto);
	int tamanho_esperando = 4*esperando;
	int espaco_livre = (int)(COLS - tamanho_esperando)/4;
	int i, j = 4, k;
	wattron(ponto, A_BOLD);
	wattron(ponto, COLOR_PAIR(2));
	for(i = 0; i < espaco_livre; i++) {		
		for(k = 0; k < 3; k++) {
			wmove(ponto, k, tamanho_esperando);
			wclrtoeol(ponto); 
		}		
		mvwprintw(ponto, 0, COLS-j, " o  ");
		mvwprintw(ponto, 1, COLS-j, "/|\\ ");
		mvwprintw(ponto, 2, COLS-j, "/ \\ ");
		refresh();
		update_panels();
		doupdate();
		usleep(60000);
		j += 4;
	}
	char message[29];
	sprintf(message, "passageiro %d chegou", id);
	print_in_history(message, COLOR_PAIR(2));
	mvwprintw(ponto, 0, tamanho_esperando, "/|\\ ");
	mvwprintw(ponto, 1, tamanho_esperando, "/ \\ ");
	mvwprintw(ponto, 2, tamanho_esperando, "    ");
	refresh();
	update_panels();
	doupdate();
	usleep(200000);
	mvwprintw(ponto, 0, tamanho_esperando, "/ \\ ");
	mvwprintw(ponto, 1, tamanho_esperando, "    ");
	refresh();
	update_panels();
	doupdate();
	usleep(200000);
	mvwprintw(ponto, 0, tamanho_esperando, "    ");
	refresh();
	update_panels();
	doupdate();
	usleep(200000);
	sprintf(message, "passageiro %d foi embora", id);
	print_in_history(message, COLOR_PAIR(2));
	wattroff(ponto, COLOR_PAIR(2));
	wattroff(ponto, A_BOLD);
	pthread_mutex_unlock(&lock_ponto);
}
