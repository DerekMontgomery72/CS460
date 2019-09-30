#define NPIPE 9

PIPE pipe[NPIPE]; // global pipe objects
PIPE *kpipe;


int pipe_init(){
  int i = 0;
  PIPE *p;
  for(i = 0; i < NPIPE ; i++){
    p = &pipe[i];
    p->status = FREE;
    p->head = p->buf;
    p->tail = p->buf;
    p->data = 0;
    p->room = PSIZE;
  }

}

PIPE *create_pipe(){
  int i = 0;
  PIPE *p;
  for(i=0; i<NPIPE ; i++){
    
    p = &pipe[i];
    if(p->status == FREE){
      p->status = BUSY;
      return p;
    }
  }
  

}

int read_pipe(PIPE *p, char buf[], int n)
{
  int r = 0;
  if(n <= 0)
    {
      return 0;
    }
  //validate PIPE pointer p; //p->status must not be FREE

  if(p->status == FREE)
    {
      printf("Invalid Pipe used\n");
      return;

    }
  while(n){
 
    while(p->data){
      *buf++ = p->buf[p->tail++]; //read a byte to buf
      p->tail %= PSIZE;
      p->data--;p->room++;r++;n--;
      if(n==0){
	break;
      }
    }
    kwakeup(&p->room);  //wakeup writers
    
    if(r){
      return r;
    }
    //pipe has no data
    ksleep(&p->data);   //sleep for data
  }
}

int write_pipe(PIPE *p, char *buf, int n)
{
  int r = 0;
  
  if(n<=0){
    return 0;
  }
  if(p->status == FREE){
    printf("Invalid Pipe used for write\n");
    return;
  }
  
  while(n){
    
    while(p->room){
      p->buf[p->head++] = *buf++; //write a byte to pipe;
      p->head %= PSIZE;
      p->data++;p->room--;r++; n--;
      if(n==0){
	break;
      }
    }
    kwakeup(&p->data);   //wakeup readers, if any
    tswitch();
    
    if(n==0){
      return r;//Finished writing n Bytes
    }
    
    //still has data to write but no room
    ksleep(&p->room);
  }
  
}

int pipe_writer()
{
  struct uart *up = &uart[0];
  char line[128];
  while(1){
    uprintf(up,"Enter a line for task1 to get : ");
    printf("task : %d waits for line from UART0\n", running->pid);
    ugets(up, line);
    uprints(up, "\r\n");
    printf("task %d : writes line=[%s] to pipe\n", running->pid, line);
    write_pipe(kpipe, line, strlen(line));

    kexit(0);
  }
}


int pipe_reader(){
  char line[128];
  int i, n;
  while(1){
    printf("task %d : reading from pipe\n", running->pid);
    n= read_pipe(kpipe, line, 20);
    printf("task %d : read n=%d bytes from pipe :[",running->pid, n);
    for(i=0; i<n; i++){
      kputc(line[i]);
    }
    printf("]\n");
    kexit(0);
  }
}

int runPipe(){

  int i = 0;
  kpipe = create_pipe();
  
  kfork((int)pipe_writer, 1); // pipe writer process
  kfork((int)pipe_reader, 1); // pipe reader process
  

}
