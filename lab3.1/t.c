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

#include "defines.h"
#include "string.c"

char *tab = "0123456789ABCDEF";
int color;

#include "vid.c"
#include "interrupts.c"
#include "kbd.c"
#include "timer.c"
//#include "string.c"
//#include "exceptions.c"
//#include "defines.h"

void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}
int kprintf(char *fmt, ...);
void timer_handler();
TIMER *tp[4];

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC SIV status registers to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;  

    if(vicstatus & (1<<4)){
      if(*(tp[0]->base+TVALUE) == 0) //timer 0
	timer_handler(0);
      if(*(tp[1]->base+TVALUE) == 0)
	timer_handler(1);
    }
      
    if(vicstatus & (1 << 5)){
      if(*(tp[2]->base+TVALUE) == 0)
	timer_handler(2);
      if(*(tp[3]->base+TVALUE) == 0)
	timer_handler(3);
    }
    if (vicstatus & (1<<31)){
      if (sicstatus & (1<<3)){
	kbd_handler();
      }
    }
}

int main()
{
   int i; 
   char line[128];

   color = YELLOW;
   row = col = 0; 
   fbuf_init();
   
   VIC_INTENABLE = 0;
   VIC_INTENABLE |= (1<<4);
   VIC_INTENABLE |= (1<<5);

   
   VIC_INTENABLE |= (1<<31); // SIC to VIC's IRQ31

   SIC_INTENABLE = 0;
   SIC_INTENABLE |= (1 <<31);
   
   timer_init();
   for(i=0; i<4; i++){
     tp[i] = &timer[i];
     timer_start(i);
   }

   kbd_init();
   
   /* enable KBD IRQ */
   SIC_ENSET = 1<<3;  // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;  // KBD int=3 on SIC
 
   kprintf("C3.2 test KBD drivers by interrupts\n");
   kprintf("enter while(1) loop\n");
   while(1);
   
}
