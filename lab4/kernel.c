/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

/********************
#define  SSIZE 1024
#define  NPROC  9
#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4
#define  printf  kprintf
 
typedef struct proc{
  struct proc *next;
  int    *ksp;
  int    *usp;
  int    status;
  int    priority;
  int    pid;
  int    ppid;
  struct proc *parent;
  int    event;
  int    exitCode;
  int    *pgdir // level-1 page table; 1MB sections only
  char   name[32];
  int    kstack[SSIZE];
}PROC;
***************************/
extern PROC *kfork();
PROC proc[NPROC], *freeList, *readyQueue, *sleepList, *running;

int procsize = sizeof(PROC);

char *pname[NPROC]={"sun", "mercury", "venus", "earth", "mars", "jupiter",
                     "saturn","uranus","neptune"};

u32 *MTABLE = (u32 *)0x4000;
int kernel_init()
{
  int i, j; 
  PROC *p; char *cp;
  int *MTABLE, *mtable;
  int paddr;

  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = FREE;
    p->priority = 0;
    p->ppid = 0;
    strcpy(p->name, pname[i]);
    p->next = p + 1;
    
  }
  proc[NPROC-1].next = 0;
  freeList = &proc[0];
  readyQueue = 0;
  sleepList = 0;
  running = dequeue(&freeList);
  running->status = READY;
  running->child = 0;
  running->sibling = 0;
  running->parent = running;

  printList(freeList);
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

int scheduler()
{
  char line[8];
  int pid; PROC *old=running;
  char *cp;
  kprintf("proc %d in scheduler\n", running->pid);
  if (running->status==READY)
     enqueue(&readyQueue, running);
  printQ(readyQueue);
  running = dequeue(&readyQueue);

  kprintf("next running = %d\n", running->pid);
  pid = running->pid;
  if (pid==1) color=WHITE;
  if (pid==2) color=GREEN;
  if (pid==3) color=CYAN;
  if (pid==4) color=YELLOW;
  if (pid==5) color=BLUE;
  if (pid==6) color=PURPLE;   
  if (pid==7) color=RED;
  // must switch to new running's pgdir; possibly need also flush TLB

  if (running != old){
    printf("switch to proc %d pgdir at %x ", running->pid, running->pgdir);
    printf("pgdir[2048] = %x\n", running->pgdir[2048]);
    switchPgdir((u32)running->pgdir);
  }
}  

int kgetpid()
{
  //kprintf("kgetpid: pid = %d\n", running->pid);
  return running->pid;
}

int kgetppid()
{
  //kprintf("kgetppid: pppid = %d\n", running->ppid);
  return running->ppid;
}
char *pstatus[]={"FREE   ","READY  ","SLEEP  ","BLOCK  ","ZOMBIE", " RUN  "};
int kps()
{
  int i; PROC *p; 
  for (i=0; i<NPROC; i++){
     p = &proc[i];
     kprintf("proc[%d]: pid=%d ppid=%d", i, p->pid, p->ppid);
     if (p==running)
       printf("%s ", pstatus[5]);
     else
       printf("%s", pstatus[p->status]);
     printf("name=%s\n", p->name);
  }
}

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


int kchname(char *s)
{ 
  kprintf("kchname: name=%s\n", s);
  strcpy(running->name, s);
  return 123;
}

int kgetPA()
{
  return running->pgdir[2048] & 0xFFFF0000;
}
