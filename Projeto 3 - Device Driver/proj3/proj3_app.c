#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>
/* #include "query_ioctl.h" */

#include <linux/ioctl.h>

typedef struct
{
  char word[500];
} query_arg_t;

#define QUERY_GET_VARIABLES _IOR('q', 1, query_arg_t *)
#define QUERY_CLR_VARIABLES _IO('q', 2)
#define QUERY_SET_VARIABLES _IOW('q', 3, query_arg_t *)

void get_vars(int fd)
{
  query_arg_t q;

  if (ioctl(fd, QUERY_GET_VARIABLES, &q) == -1)
    {
      perror("query_apps ioctl get");
    }
  else
    {
      printf("%s\n", q.word);
    }
}

void clr_vars(int fd)
{
  if (ioctl(fd, QUERY_CLR_VARIABLES) == -1)
    {
      perror("query_apps ioctl clr");
    }
}
void set_vars(int fd)
{
  query_arg_t q;
 
  printf("Enter Status: ");
  scanf("%s", q.word);
  getchar();

  if (ioctl(fd, QUERY_SET_VARIABLES, &q) == -1)
    {
      perror("query_apps ioctl set");
    }
}
 
int main(int argc, char *argv[])
{
  int fd;
  char *file_name = "/dev/query";
    enum
    {
      e_get,
      e_clr,
        e_set
    } option;
  fd = open(file_name, O_RDWR);
    if (argc == 1)
      {
        option = e_get;
      }
    else if (argc == 2)
      {
        if (strcmp(argv[1], "-g") == 0)
	  {
            option = e_get;
	  }
        else if (strcmp(argv[1], "-c") == 0)
	  {
            option = e_clr;
	  }
        else if (strcmp(argv[1], "-s") == 0)
	  {
            option = e_set;
	  }
        else
	  {
            fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
            return 1;
	  }
      }
    else
      {
        fprintf(stderr, "Usage: %s [-g | -c | -s]\n", argv[0]);
        return 1;
      }
 
    switch (option)
      {
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
 
    close (fd);
 
    return 0;
}