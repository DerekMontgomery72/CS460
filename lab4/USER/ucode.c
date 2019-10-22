int umenu()
{
  uprintf("----------------------------------------------\n");
  uprintf("getpid getppid ps chname switch exit fork wait\n");
  uprintf("----------------------------------------------\n");
}

int getpid()
{
  int pid;
  pid = syscall(0,0,0,0);
  return pid;
}    

int getppid()
{ 
  return syscall(1,0,0,0);
}

int ugetpid()
{
  int pid = getpid();
  uprintf("pid = %d\n", pid);
}

int ugetppid()
{
  int ppid = getppid();
  uprintf("ppid = %d\n", ppid);
}

int ups()
{
  return syscall(2,0,0,0);
}

int uchname()
{
  char s[32];
  uprintf("input a name string : ");
  ugetline(s);
  printf("\n");
  return syscall(3,s,0,0);
}

int uswitch()
{
  return syscall(4,0,0,0);
}

int ugetc()
{
  return syscall(90,0,0,0);
}

int uputc(char c)
{
  return syscall(91,c,0,0);
}

int getPA()
{
  return syscall(92,0,0,0);
}

int ufork()
{
  char s[32];
  char *filename;
  uprintf("Input a file Name:");
  ugetline(s);
  filename = s;
  uprintf("\n");
  
  return syscall(5,(int)filename,0,0);
}

int uexit()
{
  int exitcode;char s[32];
  uprintf("Input an Exit Code: ");
  ugetline(s);
  printf("\n");
  exitcode = atoi(s);

  syscall(7,exitcode,0,0);

}
int uwait()
{
  int pid, status;
  // pid = wait(&status);

  pid = syscall(6,&status,0,0);
  printf("Child %d, Status: %d\n",pid, status);

}
  
