
//#queue.h
//Derek Montgomery
//WSUID: 11508236


int enqueue(PROC **queue, PROC *p)
{
  PROC *q  = *queue;
  if (q==0 || p->priority > q->priority){
    *queue = p;
    p->next = q;
    return;
  }
  while (q->next && p->priority <= q->next->priority){
    q = q->next;
  }
  p->next = q->next;
  q->next = p;
}

PROC *dequeue(PROC **queue)
{
  PROC *p = *queue;
  if (p)
    *queue = p->next;
  return p;
}

int tEnqueue(TQE **queue, TQE *t)
{
  // int sr = int_off();
  TQE *q = *queue;
  if(q == 0 || q->time <= t->time){
    if(q == 0){
      *queue = t;
      t->next = 0;
      printf("Inserted Proc %d into timer Queue at front\n",t->pid);
      //  int_on(sr);
      //  ksleep((int)running->pid);
      return 0;
    }
    t->next = q;
    *queue = t;
    while(q != 0){
      q->time = q->time - t->time;
    }
    //  int_on(sr);
    // ksleep((int)running->pid);
     
     return 0;
  }
  while(q->next && q->time >= t->time){
    q=q->next;
  }
  t->next = q->next;
  q->next = t;
  t->time = t->time - q->time;
  q = t->next;
  printf("Inserted Proc %d into Timer queue between two items\n",t->pid);
  while(q != 0 ){   
    q->time = q->time - t->time;
      }
  // int_on(sr);
  // ksleep((int)running->pid);
  return;
}

TQE *tDequeue(TQE **timerQueue)
{
  TQE *t = *timerQueue;
  if(t)
    {
      *timerQueue = t->next;
    }
  return t;
}

int tDecrement(TQE **timerQueue){
  TQE *q = *timerQueue;

  q->time = q->time -1;
  q->display[4] = '0' + (q->time%10); q->display[3] = '0' + (q->time/10);
  return q->time;
}

  

int printList(char *name, PROC *list)
{
  printf("%s=", name);
  PROC *p = list;
  PROC *front = p;
  while(p){
    printf("[%d%d]->", p->pid, p->priority);
    p = p->next;
    if(p == front){
      printf("NULL \n");
      return;
    }
    if(p == p->next){
      printf("error Circular pointer\n");
      return;
    }
  }
  printf("NULL\n");
}
