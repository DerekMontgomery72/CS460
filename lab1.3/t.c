/*******************************************************
*                      t.c file                        *
*******************************************************/
//Derek Montgomery
//CPTS 460
//Lab 1


typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"

typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

DIR   *dp;
char buf1[BLK], buf2[BLK];
int color = 0x0A;
char *name[2];

main()
{
  GD *gp;
  u16 iblk;
  INODE *ip;
  int i, ino;
  u32 *up;
 
  name[0] = "boot";
  name[1] = "mtx";
  getblk(2, buf1);
  gp = (GD *)buf1;
  iblk = (u16)gp->bg_inode_table;
 
  
  color=0x0B;
 
  getblk(iblk,buf1);
  ip = (INODE *)buf1 + 1;

  for(i=0; i<2; i++){
    
    ino = FindDir(ip,name[i]);
    ino--;
     if(ino < 0)
       error();
	
     getblk(iblk + (ino/8), buf1);
     ip = (INODE *)buf1 + (ino % 8);
     
  }
  if((u16)ip->i_block[12])
    getblk((u16)ip->i_block[12],buf2);
  
  setes(0x1000);
  
  for(i=0; i<12; i++){
    getblk((u16)ip->i_block[i],0);
    inces();
    putc('*');

  }
 
  if((u16)ip->i_block[12]){
    up = (u32 *)buf2;
    while(*up){
      getblk(*up, 0);
      up++;
      inces(); putc('.');
    }
    getc();
    
  }
  return;
}


	   
int FindDir(INODE *ip, char *name)
{
  int i = 0;char c;
    
 for(i=0; i<12; i++)
     {
      
     if((u16)ip->i_block[i] )
       {
	
	 getblk(((u16)ip->i_block[i]),buf2);
		
	 dp = (DIR *)buf2;
	 
	 while((char *)dp <(buf2 + BLK))
	   {
	     c = dp->name[dp->name_len];
	     dp->name[dp->name_len] = 0;
	     
	     prints(dp->name);
	     prints("\n\r");
	     
	     if(mycompare(name,dp->name) == 0)
	       {
		 dp->name[dp->name_len] = c;
		 getc();
		 return (u32)dp->inode;
		 
	       }
	     dp->name[dp->name_len] = c;
	     dp = (char *)dp + dp->rec_len;
	   }
       }
     }
 return -1;
}

int mycompare(char *s1, char *s2){
  while(*s1 != '\0' && *s2 != '\0')
    {
      if(*s1 == *s2){
	s1++;
	s2++;
	continue;
      }
      else if(*s1 < *s2){
	return -1;
      }
      else{
	return 1;
      }
    }
  return 0;
}

int prints(char *s)
{
  while(*s != '\0'){
    putc(*s);
    s++;
  }
  return 0;
}

/*
int rpu(u32 x){
  char c; char *ctable = "0123456789ABCDEF";
  if(x){
    c = ctable[x % 10];
    rpu(x/10);
    putc(c);
  }
}
  
int printu(u32 x){
  
  (x==0)? putc('0') : rpu(x);
  putc(' ');
}
*/
/*
int gets(char *s) 
{
    int x = 0;
  while(x < 63){
   
   s[x]= getc();
   if(s[x] == '\r'){
     s[x] = '\0';
     prints("\n\r");
     return 0;
   }
    putc(s[x]);
    x++;
  }
  s[x] = '\0';
  prints("\n\r");
  return 0;
}
*/

int getblk(u16 blk, char *buf)
{
  readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
  // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

