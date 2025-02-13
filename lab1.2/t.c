/*******************************************************
*                      t.c file                        *
*******************************************************/
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
GD    *gp;
INODE *ip;
DIR   *dp;

char buf1[BLK], buf2[BLK];
int color = 0x0A;
u8 ino;

char* path= "/boot/mtx";



main()
{ 
  u16    i, iblk;
  char   c, temp[64];
  char *cp;
  
  prints("read block# 2 (GD)\n\r");
  
  getblk(2, buf1);
  gp = (GD *)buf1;
  iblk = (u16)gp->bg_inode_table;
  

// 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
  prints("inode_block="); putc(iblk+'0'); prints("\n\r");
  color=0x0B;
  getc();

// 2. WRITE YOUR CODE to get root inode
  prints("read inodes begin block to get root inode\n\r");
  getblk(iblk,buf1);
  
   prints("read data block of root DIR\n\r");
   ip = (INODE *)buf1 + 1;

   printRoot(ip);
  
}
	   
int printRoot(INODE *ip){
 for(i=0; i<12; i++)
     {
     if((u16)ip->i_block[i] )
       {
       color=0x0A;
       prints("read data block(s) of root Dir\n\r");
       getblk(((u16)ip->i_block[i]),buf2);
       cp = buf2;
       dp = (DIR *)buf2;
       
       while(cp <(buf2 + BLK))
	 {

	 prints(dp->name);
	 prints("\n\r");

	 cp+= dp->rec_len;
	 dp = (DIR *)cp;
	 }
       
     }
     
   }

}

int mycompare(char *s1, char *s2){
  while(*s1 != '\0' && *s2 != '\0')
    {
      if(*s1 == *s2){
	continue;
      }
      else if(*s1 < *s2){
	return -1;
      }
      else{
	return 1;
      }

    }
}

char* mystrtok(char s[], char c)
{
  char *c = s[0], *ret = s[0];
  while(*c != '\0'){
    if(*c == c){
      *c = '\0';
      *c++;
    }
  }
  return ret;
}

int prints(char *s)
{
    // int c = 0;
  while(*s != '\0'){
    putc(*s);
    s++;
  }
  return 0;
  
  // call putc(c) to print string s;
}

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
  // call getc() to input a string into s[ ]
}

int getblk(u16 blk, char *buf)
{
  // readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
  readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

