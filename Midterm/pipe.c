#define NPIPE 9
PIPE pipe[NPIPE]; // global pipe objects



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
      *buf++ = p->buf[p->tail++]; //read a byt to buf
      p->tail %= PSIZE;
      p->data--;p->room++;r++;n--;
      if(n==0){
	break;
      }
    }
    wakeup(&p->room);  //wakeup writers
    if(r)
      return r;
    //pipe has no data
    sleep(&p->data);   //sleep for data
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
    wakeup(&p->data);   //wakeup readers, if any
    if(n==0){
      return r;
    }
    
    //still has data to write but no room
    sleep(&p->room);
  }
  
}

int wakeup(char *data){

}
int sleep(int room){

}
