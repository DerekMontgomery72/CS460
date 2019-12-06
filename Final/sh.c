#include "type.h"
#include "ucode.c"

char homedir[256];
char *stdin = "/dev/tty0";

int main(int argc, char *argv[ ])
{
  int pid, status;
  char cmd[256];
  getcwd(homedir);

  while(1){
    //display executable commands in /bin directory
    printf("Enter a command %c\n",'\0');
    gets(cmd);
    //prompt for a command line cmdline = "cmd a1 a2 .. an"
    if(!strcmp(cmd,"exit")){
      exit(0);
    }
    else if(mystrContains(cmd, "cd "))
    {
      char *cp;
      if(strlen(cmd) > 3)
      {
        cp = cmd[2];
        chdir(cp);
      }
      else
      {
        chdir(homedir);
      }
      
    }
      //fork a child process to execute the cmd line
    pid = fork();
    if (pid){
      pid = wait(&status);
    }
    else{
      if(!checkInput(cmd))
      {
        executeInput(cmd);
      }
    }
  }
}
  

  int checkInput(char *input)
  {
    if(input[strlen(input) - 1] == '\n')
    {
      input[strlen(input) -1] = 0;
    }
    if(strcmp(input, "") == 0)
    {
      printf("Please input a command%c\n",'\0');
      return 1;
    }
    return 0;
  }
int HasPipe(int *index, char *incmd)
{
int i, length;
char cmd[256];
strcpy(cmd,incmd);
length = strlen(cmd);

    for(i = 0; i < length; i++)
    {
      if(cmd[i] == '|')
      {
        *index = i;
        printf("Has pipe \n%c",'\0');
        return 1;
      }
    }
    return 0;

}

int HasRedirect(char *incmd, int *index)
{

  int i;
  char cmd[256];
  strcpy(cmd,incmd);
  for(i = 0; i < strlen(cmd); i++)
  {
    if(cmd[i] == '<'){
      *index = i;
      return 1;
    }
    else if(cmd[i] == '>')
    {
      *index = i;
      printf("Index of Redirect =%d\n",*index);
      if((i+3) < strlen(cmd))
      {
        if(cmd[i+1] == '>')
        {
          return 2;
        }
        else
        {
          return 3;
        }
      }
    }
  }
  return 0;
}

int Exec(char *str)
{
  int *index, redirectVal, length, ind, file;
  char *cp, *cp1;
  cp =str;
  index = &ind;
  length = strlen(str);
  redirectVal = HasRedirect(str, index);
  printf("Redirect Value: %d\n",redirectVal);
  if(redirectVal == 0)
  {
    exec(str);
  }
  else if(redirectVal == 1)
  {
    // Redirect = '<'
    if(*index + 2 < length)
    {
      str[*index - 1] = '\0';
      str[*index + 1] = '\0';
      cp1 = &str[*index+2];
      printf("Has < redirect: command : %s file: %s\n",cp, cp1);
      close(1);
      
      file = open(cp1, O_RDONLY);
      dup2(file,1);

      exec(cp);
      close(1);
      close(file);
      file = open(stdin, O_RDONLY);
    }
  }
  else if(redirectVal == 2)
  {
    // Redirect = '>>'
    printf("index value in execute: %d\n",*index);

    if(*index + 3 < length)
    {
      str[*index -1] = '\0';
      str[*index + 2] = '\0';
      cp1 = &str[*index+3];
      printf("Has >> redirect: command : %s file: %s\n",cp, cp1);
      
      close(0);
      file = open(cp1, O_WRONLY|O_CREAT);
      dup2(file,0);
      exec(cp);
      close(0);
      close(file);
      file = open(stdin, O_WRONLY);

      dup2(file,0);
    }
  }
  else if(redirectVal == 3)
  {
    // Redirec = '>'
    printf("index value in execute: %d\n",*index);
    if((*index)+ 2 < length)
    {

      str[*index - 1] = '\0';
      str[*index + 1] = '\0';
      cp1 = &str[*index+2];

      printf("Has > redirect: command : %s file: %s\n",cp, cp1);

      close(1);
      file = open(cp1, O_APPEND|O_WRONLY|O_CREAT);
      dup2(file,0);
      exec(cp);
      close(0);
      close(file);
      file = open(stdin,O_WRONLY);
      dup2(file, 0);
    }
  }
  else
  {
    printf("There was an error in shell%c\n",'\0');
  }
  
}

int executeInput(char *input)
{
  int pipe = 0, i, pipeindex = 0, redirect = 0, length = 0, *index;
  char *cp, *cp1;
  length = strlen(input);
  cp = input;
  if(HasPipe(index, input))
  {
    pipe = 1;
   if((*index+1) < length)
      {
        input[*index+1] = '\0';
      }
      input[*index] = '\0';
      input[*index-1] = '\0';
    
      if((*index+2) < length)
      {
       cp1 = &input[*index+2];
      }
      else
      {
        printf("Invalid Pipe\n%c",'\0');
        return 1;
      }
  }
  else
  {
    Exec(input);
  }
return;
}

int ExecutePipe(char *cmd1, char *cmd2)
{
  int pd[2], pid, *index, length;
  char *cp1, *cp2;
  
  pipe(pd); 
  pid = fork();
  if(pid == 0)
  {
    close(0);
    dup2(pd[1],0);
    close(pd[0]);
    exec(cmd1);
  }
  else
  {
    cp1 = cmd2;
    length = strlen(cmd2);
    close(1);
    dup2(pd[0],1);
    close(pd[1]);


    if(HasPipe(index, cmd2))
    {
      if((*index+1) < length)
      {
         cmd2[*index+1] = '\0';
      }
        cmd2[*index] = '\0';
        cmd2[*index-1] = '\0';
      
        if((*index+2) < length)
        {
          cp2 = &cmd2[*index+2];
        }
        else
        {
          printf("Invalid Pipe\n%c",'\0');
          return 1;
        }
        ExecutePipe(cp1, cp2);
    }
    else
    {
      close(0);
      open("/dev/tty0",O_WRONLY);
      exec(cmd2);
    }
    
  }
}
