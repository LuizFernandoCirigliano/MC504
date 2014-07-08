#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
 
#ifndef QUERY_IOCTL_H
#define QUERY_IOCTL_H
#include <linux/ioctl.h>

typedef struct
{
	int count[26];
	char text[500];
} query_arg_t;
 
#define QUERY_GET_COUNT _IOR('q', 1, query_arg_t *)
#define QUERY_CLR_TEXT _IO('q', 2)
#define QUERY_SET_TEXT _IOW('q', 3, query_arg_t *)
 
#endif
 
void get_vars(int fd)
{
	int i;
	query_arg_t q;
	if(ioctl(fd, QUERY_GET_COUNT, &q) == -1) {
		perror("Erro ioctl ao contar letras.");
	}
	else {
		for(i = 0; i < 26; i++) {
			printf("%c: %d\n", (char)i, q.count[i]);		
		}
	}
}
void clr_vars(int fd)
{
	if(ioctl(fd, QUERY_CLR_TEXT) == -1) {
		perror("Erro ioctl ao zerar variaveis.");
	}
}
void set_vars(int fd)
{
	query_arg_t q;
	printf("Digite um texto(ate 500 caracteres): ");
	scanf("%s", q.text);
	if(ioctl(fd, QUERY_SET_TEXT, &q) == -1) {
	  perror("Erro ioctl ao setar texto.");
	}
}
 
int main(int argc, char *argv[])
{
	char *file_name = "/dev/query";
	int fd;
	enum {e_get, e_clr, e_set} option; 
	if(argc == 1) {
	  option = e_get;
	}
	else if(argc == 2) {
		if(strcmp(argv[1], "-g") == 0) {
			option = e_get;
		}
		else if(strcmp(argv[1], "-c") == 0) {
			option = e_clr;
		}
		else if (strcmp(argv[1], "-s") == 0) {
			option = e_set;
		}
		else {
			fprintf(stderr, "Erro - flags: %s [-g | -c | -s]\n", argv[0]);
			return 1;
		}
	}
	else {
		fprintf(stderr, "Erro - flags: %s [-g | -c | -s]\n", argv[0]);
		return 1;
	}
	fd = open(file_name, O_RDWR);
	if(fd == -1) {
		perror("Erro ao abrir arquivo!!!");
		return 2;
	} 
	switch(option) {
	  case e_get:
	  		get_vars(fd);
		   break;
	  case e_clr:
		   clr_vars(fd);
		   break;
	  case e_set:
		   set_vars(fd);
		   break;
	  default:
		   break;
	} 
	close(fd);
	return 0;
}
