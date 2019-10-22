
// Wait.C FIle
// Derek Montgomery
// Student ID 11508236



int ksleep(int event)
{
  //record event value in running PROC.event;
  //change runnng PROC status to SLEEP;
  //switch process
  int sr = int_off();
  
  printf("proc %d going to sleep on event=%x\n", running->pid, event);
  
  running->event = event;
  running->status = SLEEP;
  printf("Put to Sleep\n");
  
  int_on(sr);
  
  tswitch();
   
}

int kwakeup(int event)
{
  int i;
  int sr = int_off();
  PROC *p;

  
  //for each PROC *p do{
  //if(p->status = SLEEP && p->event == event){
  //   change p->status to reader;
  //   enter p into redyQueue

  
  for(i=0; i< NPROC; i++)
    {
      p= &proc[i];
      if(p->status == SLEEP && p->event == event){
	p->event = 0;
	p->status = READY;
	enqueue(&readyQueue,p);
	printf("Wakeup Process %d\n",p->pid);
      }
      
    }
  
  int_on(sr);
  // tswitch();
}

      
int kexit(int exitCode)
{

  //1. erase process user-mode context, e.g close file descriptors, relase resources, deallocate user-mode image  memory, etc.
  //2. dispose of children processes, if any.

  //3. record exitValue in PROC.exitcode for parent to get.

  //wakeup parent and, if needed also the INIT process P1.

  //6. switch process to give up CPU

  printf("In kexit()\n");
  
  int i, P1 = 0;
  PROC *p = running;
  if(p->pid == 0 && NPROC > 2){
    printf("processes stil running, can't kill p1\n");
    return;
  }
  
  P1 = sendChild();
  printf("children sent to p1");

  running->exitCode = exitCode;
  running->status = ZOMBIE;
 
  kwakeup((int)running->parent);
  
  if(P1){
    kwakeup(&proc[1]);
  }
  tswitch();
}

int kwait(int *status)
{
  int i;
  PROC *p, *child;
  p = running;

  if (p->child == 0){
    printf("wait error: no child\n");
    return -1;
  }
  child = p->child;

  while(1)
    {
      // if can find a ZOMBIE child
      //free ZOMBIE PROC;
      //  return its pid and exitcode
      printf("In Wait\n");
      child = p->child; //set child to beginning of children list
      while(child) //search through children
	{
	  if(child->status == ZOMBIE)//found zombie child
	    {
	      printf("Found ZOMBIE\n");
	      child->status = FREE;
	      
	      enqueue(&freeList,child);
	      
	      delete_child(child);
	      
	      *status = child->exitCode;
	      return child->pid;
	      
	    }
	  child = child->sibling; //move to next child
	}
      printf("Going to Sleep\n");
      ksleep((int)running);// No zombies found, sleep
    }
}


int timerInsert(int time, TQE *t){
  // int sr= int_off();
  tEnqueue(&timerQueue, t);
  printf("PROC %d inserted into TimerQueue\n",timerQueue->pid);
  //  int_on(sr);
  //  ksleep((int)running->pid);
  //tswitch();
  return 0;

}
