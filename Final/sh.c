#include "ucode.c"


int main(int argc, char *argv[ ])
{
  int pid, status;
  while(1){
    //display executable commands in /bin directory
    //prompt for a command line cmdline = "cmd a1 a2 .. an"
    if(!strcmp(cmd,"exit")){
      exit(0);
    }
      //fork a child process to execute the cmd line
    pid = fork();
    if (pid){
      pid = wait(&status);
    }
    else{
      exec(cmdline);   //"cmd a1 a2 ... an");
    }
  }
  
