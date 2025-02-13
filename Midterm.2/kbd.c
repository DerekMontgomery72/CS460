/************* kbd.c file **************/

//#include "keymap"
#include "keymap2"

#define KCNTL 0x00
#define KSTAT 0x04
#define KDATA 0x08
#define KCLK  0x0C
#define KISTA 0x10

#define CAPSLOCK 0x58
#define LSHIFT   0x12
#define RSHIFT   0x36
#define CONTROL  0x14


typedef volatile struct kbd{
  char *base;
  char buf[128];
  int head, tail, data, room;
}KBD;

volatile KBD kbd;
int count,third;
int shift;

int control;
int capslock;
int release;

char *number = "0123456789";

int kbd_init()
{
  char scode;
  KBD *kp = &kbd;
  kp->base = (char *)0x10006000;
  *(kp->base + KCNTL) = 0x10; // bit4=Enable bit0=INT on
  *(kp->base + KCLK)  = 8;
  kp->head = kp->tail = 0;
  kp->data = 0; kp->room = 128;
  count = 0;  // number of KBD interrupts
  third = 0;
  shift = 0;
  control = 0;
  capslock = 0;
  release = 0;
}

void kbd_handler()
{
  
  unsigned char code, c;
  int i, n = 0;
  KBD *kp = &kbd;
  code = *(kp->base + KDATA);
  
  if(code & 0x80){ // key release: Only catch release on shift, control
    release = 1;
    
    return;
  }
  if(release){
    if(code == LSHIFT || code == RSHIFT){
      shift = 0;
    }
    if(code == CONTROL)
      control = 0;
    release = 0;
  }  
  
  if(third == 0){
  
    //from here on, must be key press
    if(code == LSHIFT || code == RSHIFT){
      if(shift == 0)
	shift = 1;
      else if(shift == 1){
	shift = 0;
	release = 0;
	return;
      }
      
      third++;
      return;
    }
    if(code == CONTROL){
      if(control == 0)
	control = 1;
      else if(control == 1)
	control = 0;
      third++;
      return;
    }
    if(code == CAPSLOCK){
      //handle capslock
      if(capslock == 0)
	capslock = 1;
      else if(capslock == 1)
	capslock = 0;
      
      third++;
      return;
    }
    
    c = (shift ? utab[code] : ltab[code]);
    
    // Code to handle caps lock key
    if(capslock){
      for( i=0; i < 10; i++){
	if(number[i] == c){
	  n = 1;
	  break;
	}
      }
      if(n == 0)
	c = (shift ? ltab[code] : utab[code]);
    }
    
    
    //code to recognize control-c key
    
    
    if((control == 1 && c == 'c') || (control == 1 && c == 'C')){
      kprintf("Control-c key\n");
      kgetc();
      c = '\n'; // force a line, let proc handle #2 Signal when exit Kmode
    }
    if((control == 1 && c == 'd') || (control == 1 && c == 'D')){
      c = 4; // Control-D
    }
    //stor char in buf[] for task to get
    if(kp->data == 128) // buf FULL, ignore current key
      return;
    
    
    kp->buf[kp->head++] = c;
    kp->head %= 128;
    kp->data++;
    
  
    color = YELLOW;
    lock();
    
    count++;
    printf("%c", c);
    release = 0;
  }
  third++;
  if(third > 1){
    third = 0;
  }
  unlock();
  
  
}



int kgetc()
{
  char c;
  KBD *kp = &kbd;
  while(kp->data == 0);
  lock();
  c= kp->buf[kp->tail++];
  kp->tail %=128;
  kp->data--; kp->room++;
  unlock();
  return c;
}

int kgets(char *s)
{
  char c;
  if(c = kgetc())
    {
      
      while(c != '\r' && c && c!= '\0' ){
       
	*s = c;
	s++;
	c = kgetc();
      }
     
      *s='\0';
    }
  return;
}

