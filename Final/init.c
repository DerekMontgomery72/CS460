#include "type.h"
#include "ucode.c"

int console;
int parent()
{
  int pid, status;
  while(1)
    {
      printf("INIT : wait for Zombie child\n");
      pid = wait(&status);
      if(pid == console)
	    {
	      printf("INIT: forks a new console login\n");
	      console = fork(); // fork another one
	      if(console)
	      {
	        continue;
	      }
	      else
	      {
	        exec("login /dev/tty0"); // new console login process
	      }
	    }
      printf("INIT: I just buried an orphan child proc %d\n",pid);
    }
}

int main()
{
  char c;
  int in, out; //file descriptors for terminal I/O
  in = open("/dev/tty0", O_RDONLY); // file descriptor 0
  out = open("/dev/tty0", O_WRONLY); // for display to console
  printf("INIT : fork a login prc on console\n");
  console = fork();
  if(console) // parent
  {
    parent();
  }
  else
  {
    exec("login /dev/tty0");
  }
}
