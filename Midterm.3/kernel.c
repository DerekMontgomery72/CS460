
//#kernel.c
//Derek Montgomery
//WSUID: 11508236

#define NPROC 9


PROC proc[NPROC], *freeList, *readyQueue, *sleepList;
int procsize = sizeof(PROC);

int ksleep(int event);
int body();

int init()
{
  int i, j;
  TQE *t; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = READY;
    p->next = p + 1;
  }
  for(i=0; i<NTIME; i++)
    {
      t = &timers[i];
      t->event = 0;
      t->sleeping = 0;
      t->pid = 0;
      t->time = 0;
      t->next = 0;
      strcpy(t->display,"[0,00]->");
    }
  proc[NPROC-1].next = 0; // circular proc list
  freeList = &proc[0];
  readyQueue = 0;
  sleepList = 0;
  
  printList("freeList", freeList);
  
  // creat P0 as initial running process
  p = (PROC *)dequeue(&freeList);
  p->priority = 0;
  p->ppid = 0;
  p->parent = p;
  p->child = 0;
  p->sibling = 0;

  running = p;

  kprintf("running = %d\n", running->pid);
  
}

int enter_child(PROC *p)
{
  // enter p into child list of parent
  p->parent = running;
  p->ppid = running->pid;
  if(running->child != 0){
    
    PROC *temp = running->child;
    while(temp->sibling){
      temp = temp->sibling;
    }
    temp->sibling = p;
  }
  else{
    running->child = p;
  }
  return 0;
  
}

char *status[ ] = {"FREE", "READY", "SLEEP", "BLOCK", "ZOMABIE"};  

int printChild()
{
  // print child list of running PROC
  if(running->child == 0){
    printf("NO children to print\n");
    return 0;

  }
  PROC *child = running->child;
  
  while(child){
    printf("[ %d  %d] ->",child->pid, child->priority);
    child = child->sibling;
  }
  printf("NULL\n");
  return 0;
  
}

int delete_child(PROC *p)
{
  // delete child p from running's childList
  PROC *parent = p->parent, *after;
  PROC *before = parent->child;
  if(p->pid == 1){
    printf("Cannot delete P1\n");
    return 1;
  }
  if(p->sibling)
    {
      after = p->sibling;
    }
  
  else{
    after = 0;
  }
  printf("before deletion loop\n");
  if(before == p){
    //item is the first in the list
    parent->child = before->sibling;
    return 0;
  }
  while(before->sibling != p)
    {
      before = before->sibling;
    }
  printf("Deleting from sibling list done\n");
  before->sibling = after;
  return 0;
  
}

int sendChild(){
  
  int procSent = 0;
  PROC *p = running, *p1 = &proc[1], *child = running->child;
  if(running->child == 0){
    printf("No children to send to p1\n");
    return 0;
  }
  while(child) // set childrens parent to p1
    {
      child->parent = p1;
      p->ppid = 1;
      child = child->sibling;
      procSent++;
    }
    
  if(p1->child){
    p1 = p1->child;
    while(p1->sibling){
      p1 = p1->sibling;
    }
    p1->sibling = p->child;
    printf("Gave children to p1");
    return procSent; 
  }
  else{
    p1->child = p->child;
    return procSent;
  }

}
  
PROC *kfork(int func, int priority)
{
  int i;
  PROC *p = (PROC *)dequeue(&freeList);
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;

  p->parent = running;
  p->child = 0;
  p->sibling = 0;

  enter_child(p);
  
  // set kstack to resume to body
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);
  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  return p;
}

int scheduler()
{
  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
    enqueue(&readyQueue, running);
  running = (PROC *)dequeue(&readyQueue);
  kprintf("next running = %d\n", running->pid);
}  

int menu()
{
  printf("------------ menu ---------------\n");
  printf("switch fork exit wait pipe tsleep\n");
  printf("---------------------------------\n");
}  
    
int do_exit()
{
  char s[16];
  int exitValue;
  if (running->pid == 1){
    printf("P1 never dies\n");
    return -1;
  }
  printf("enter an exit value : ");
  kgets(s);
  exitValue = atoi(s);
  kexit(exitValue);
  
}

int do_wait()
{
  int pid, status;
  printf("proc %d wait for a ZOMBIE child\n", running->pid);
  pid = kwait(&status);
  printf("proc %d wait for a ZOMBIE child %d ststus=%d\n",
	 running->pid, pid, status);
}

TQE *get_TQE(){
  int i;
  TQE *t;
  for(i = 0; i<NTIME; i++){
    t = &timers[i];
    if(t->pid == 0){
      return t;
    }
  
  }
  printf("No available procs to put to sleep\n");
  return 0;

}

int do_timer()
{
  int command[20];
  char sTime[14];
  int timeToSleep;
  
  while(1){
    printf("Please enter a time for new entry");
    kgets(sTime);
    if(readyQueue->pid == 0){
      kfork((int)do_timer,1);
    }
    
 
  printf("\n");
  int sr = int_off();
  timeToSleep = atoi(sTime);
  printf("Time For sleeping: %d\n",timeToSleep);
  
  TQE *t = get_TQE();
  if(t == 0){
    printf("Error unable to find open spot in queue\n");
    return 1;
  }
  
  t->sleeping = running;
  t->event = (int)running->pid;
  t->time = timeToSleep;
  t->pid = running->pid;
  t->display[2] = (t->pid/10);

 
  
  running->status = SLEEP;
  running->event = t->event;
  tEnqueue(&timerQueue,t);
  // tswitch();
  int_on(sr);
  tswitch();
  tswitch();
  printf("Before TSwitch()\n");
  printList("readyQueue", readyQueue);
  tswitch();
  // timerInsert(timeToSleep,t);

  }
  return 0;
  
  
}


int body()
{
  char command[64];
  int pid, status;
  kprintf("proc %d resume to body()\n", running->pid);

  
  while(1){

    pid = running->pid;
    if (pid==0) color=BLUE;
    if (pid==1) color=GREEN;
    if (pid==2) color=CYAN;
    if (pid==3) color=YELLOW;
    if (pid==4) color=WHITE;
    if (pid==5) color=GREEN;
    if (pid==6) color=CYAN;   
    if (pid==7) color=YELLOW;
    if (pid==8) color=WHITE;
  
    kprintf("proc %d running:  ", running->pid);
    printf("\n");
    printf("Children : ");
    printChild();
    printList("freeList  ", freeList);
    printList("readyQueue", readyQueue);
    
    menu();
    
    kgets(command);
    
    printf("COMMAND: %s : \n",command);
    
    if (strcmp(command, "switch")==0)
      tswitch();
    if (strcmp(command, "fork")==0)
      kfork((int)body, 1);
    if (strcmp(command, "exit")==0)
      do_exit();
    if (strcmp(command, "wait")==0)
      do_wait();
    if (strcmp(command, "pipe")==0)
      runPipe();
    if(strcmp(command, "tsleep")==0){
      timer_start(0);
      do_timer();
    }
  }
}

int cSleep()
{
  kgetc();
  return 0;

}

