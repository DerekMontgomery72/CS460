//timer register u32 offsets from base address
#define TLOAD   0x0
#define TVALUE  0x1
#define TCNTL   0x2
#define TINTCLR 0x3
#define TRIS    0x4
#define TMIS    0x5
#define TBGLOAD 0x6



volatile TIMER timer[4]; //4 timers, 2 per unit

int kwakeup(int event);

void timer_init()
{
  int i; TIMER *tp;
  printf("timer_init()\n");
  for(i=0; i < 4; i++)
    {
      tp = &timer[i];
      if(i==0) tp->base = (u32 *)0x101E2000;
      if(i==1) tp->base = (u32 *)0x101E2020;
      if(i==2) tp->base = (u32 *)0x101E3000;
      if(i==3) tp->base = (u32 *)0x101E3020;
      *(tp->base+TLOAD) = 0x0;
      *(tp->base+TVALUE)= 0xFFFFFFFF;
      *(tp->base+TRIS)  = 0x0;
      *(tp->base+TMIS)  = 0x0;
      *(tp->base+TLOAD) = 0x100;

      *(tp->base+TCNTL) = 0x66;
      *(tp->base+TBGLOAD) = 0x1C0;
      tp->tick = tp->hh = tp->mm = tp->ss = 0;
      strcpy((char *)tp->clock, "00:00:00");
    }
}

void timer_handler(int n){
  int i, preColor;
  TIMER *t = &timer[n];
  char c;
  lock();
  for(i=0;i<8; i++){
    c = t->clock[i];
   unkpchar(c,n,70+i);
  }
  t->tick++;
  if(t->tick == 120){
    t->tick = 0; t->ss++;
    timerQueueHandler();
    if(t->ss ==60){
      t->ss = 0; t->mm++;
      
      if(t->mm == 60){
	t->mm = 0; t->hh++;
      }
    }
    t->clock[7] = '0' + (t->ss%10); t->clock[6] = '0' + (t->ss/10);
    t->clock[4] = '0' + (t->mm%10);t->clock[3] = '0' + (t->mm/10);
    t->clock[1] = '0' + (t->hh%10); t->clock[0] = '0' + (t->hh/10);
  }
  preColor = color;
  color = GREEN;
  for(i=0; i<8; i++){
    c = t->clock[i];
    kpchar(c, n, 70+i);
  }
  color = preColor;
  timer_clearInterrupt(n);
  unlock();
}

void timer_start(int n)
{
  TIMER *tp = &timer[n];
  kprintf("timer_start %d base= %x\n",n, tp->base);
  *(tp->base+TCNTL) |= 0x80;
}

int timer_clearInterrupt(int n)
{
  TIMER *tp = &timer[n];
  *(tp->base+TINTCLR)= 0xFFFFFFFF;
}

void timer_stop(int n)
{
  TIMER *tp = &timer[n];
  *(tp->base+TCNTL) &=0x7F;
}



int timerQueueHandler(){

  //Decrement first item in timer queue
  //If time remaining return
  //else dequeue element from queue
  ////wakeup process
  //check next process if time remaining is zero wake it up as well
  int i,start = 50;
  if(timerQueue != 0){
    int time = tDecrement(&timerQueue);
    TQE *t;
    t = timerQueue;

    while(t != 0){
      
      printf("[%d %d] ->",t->pid, t->time);

      start+= strlen(t->display);
      t = t->next;
      
    }
    printf("\n");
    start = 50;
    
    //  printf("Proc %d Asleep Time Remaining: %d\n",timerQueue->pid, timerQueue->time);
    while(time <= 0){
    
      t = tDequeue(&timerQueue);
      kwakeup(t->event);
      t->sleeping = 0;
      t->pid = 0;
      t->event = 0;
      if(timerQueue == 0){
	return;
      }
      time = timerQueue->time;
    }
    t = timerQueue;
    
    while(t != 0){
      
      for(i=0; i<10; i++){
	kpchar(t->display[i],5,start+i);
      }

      start+= strlen(t->display);
      t = t->next;
      
    }
    
  }
  
  return 0;
  
}
