// uart.c file

#define DR   0x00
#define FR   0x18

#define RXFE 0x10
#define TXFF 0x20



int uart_init()
{
  int i; UART *up;
  for(i=0; i <4; i++)
    {
      up = &uart[i];
      up->base = (char *)(0x101F1000 + i*0x1000);
      up->n = i;
    }
  uart[3].base = (char *)(0x10009000);

  // ARM versatilepb's 4 UARTs are at the base addresses
  //  0x101F1000, 0x101F2000, 0x101F3000, 0x10009000
  // Write code to initialize the 4 UART structs.
}

int ugetc(UART *up)
{
  while(*(up->base+FR) & RXFE);
  return *(up->base+DR);
  // up points at a UART struct;
  // Write code to return an input char from the UART
}

int uputc(UART *up, char c)
{
  while(*(up->base+FR) & TXFF);
  *(up->base+DR) = c;
  // up points at a UART struct;
  // Write code to output a char to the UART
}

int ugets(UART *up, char s[ ])
{
  lock();
  // Write code to input a string from a UART
  
  while((*s = ugetc(up)) != '\r')
    {
      
      uputc(up,*s);
      s++;
    }
  *s=0;
  unlock();
}

int uprints(UART *up, char *s)
{
  while(*s){
    uputc(up,*s++);
  // Write code to print a string s to UART
  }
}
int urpx(UART *up, int x)
{
  char c;
  if(x){
    c = tab[x % 16];
    urpx(up, (x /16));
  }
  uputc(up, c);
}

int uprintx(UART *up,int x)
{
  uputc(up, '0'); uputc(up,'x');
  if(x==0)
    uputc(up, '0');
  else
    urpx(up,x);
  uputc(up, ' ');
}

int urpu(UART *up, int x)
{
  char c;
  if(x){
    c = tab[x % 10];
    urpu(up, (x /10));
  }
  uputc(up,c);
}

int uprintu(UART *up, int x)
{
  if(x==0){
    uputc(up,' ');uputc(up,'0');
  }
  else
    urpu(up, x);
  uputc(up,' ');
}

int uprinti(UART *up, int x){

  char c;
  if(x){
    c = tab[x % 10];
    urpu(up, (x /10));
  }
    uputc(up,c);

}


// major work:
int uprintf(UART *up, char *fmt, ...)
{

  int *ip;
  char *cp;
  cp = fmt;
  ip = (int *)&fmt +1;
  while(*cp){
    if(*cp != '%'){
      uputc(up,*cp);
      if(*cp =='\n')
	uputc(up,'\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
    case 'c':uputc(up,(char)*ip); break;
    case 's' : uprints(up,(char *)*ip); break;
    case 'd': uprinti(up,*ip); break;
    case 'u': uprintu(up,*ip);break;
    case 'x': uprintx(up,*ip);break;
    }
    cp++;
    ip++;
  }
  // Write code to print char, string, unsigned integer, integer
  //                by   %c     %s           %x           %d
}
