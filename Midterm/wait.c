int ksleep(int event)
{
  int sr = int_off();
    printf("proc %d going to sleep on event=%x\n", running->pid, event);
    YOUR CODE here
  int_on(sr);
}

int kwakeup(int event)
{
  int sr = int_off();
   YOUR CODE here
  int_on(sr);
}

      
int kexit(int exitCode)
{
  int i;
  PROC *p;

  sendChild();  // give children to P1

  running->exitCode = exitCode;
  running->status = ZOMBIE;
 
  kwakeup((int)running->parent);
  tswitch();
}

int kwait(int *status)
{
  int i;
  PROC *p;

  if (NO CHILD){
    printf("wait error: no child\n");
    return -1;
  }

  while(1){
    // if can find a ZOMBIE child
    {
        free ZOMBIE PROC;
        return its pid and exitcode
    }
    ksleep((int)running);
  }
}


