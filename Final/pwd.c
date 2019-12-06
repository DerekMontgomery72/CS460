
/**** globals defined in main.c file ****/
/*
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;
extern OFT    NOFT[NFD],SNOFT[NFD];
extern char   gpath[256];
extern char   names[16][256];
extern int    n;
extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;
extern char line[256], cmd[32], pathname[256], name[64];
char pwdline[256];

#define OWNER  000700
#define GROUP  000070
#define OTHER  000007






int rpwd(MINODE *wd);
  
int pwd(MINODE *wd)
{
  if(wd == root){
    printf("/\n");
    strcpy(pwdline,"/");
    return 0;
  }
  strcpy(pwdline,"\0");
  rpwd(wd);
  printf("\n");
  printf("pwdLine: %s\n",pwdline);
  return 0;
}


int rpwd(MINODE *wd){
   char buf[BLKSIZE];
   char myname[64];
   DIR *dp;
   char *cp;
   int i;
   int myino, parentino;
   MINODE *pip;
  
   
   if(wd==root){
     return;
   }

 int succ = get_block(dev,wd->INODE.i_block[0],buf);
 if(succ == -1){
   printf("Unable to get the block into the buffer\n");
   exit(1);
 }
   cp = buf;
   dp = (DIR *)buf;
   strncpy(myname,dp->name,dp->name_len);
   myname[dp->name_len] = 0;
   printf("first name: %s\n",myname);
   myino = dp->inode;
   printf("myino %d\n",myino);
   cp += dp->rec_len;
   dp = (DIR *)cp;

   parentino = dp->inode;
   printf("parent ino: %d\n", parentino);
   pip = iget(dev,parentino); //RefCount++
   get_block(dev,pip->INODE.i_block[0],buf);
   iput(pip);

   cp = buf;
   dp = (DIR *)buf;
   while( cp < (BLKSIZE + buf)){
     if(dp->inode == myino){
       strncpy(myname,dp->name,dp->name_len);
       myname[dp->name_len] = 0;
       printf("my Name %s\n",myname);
       break;
     }
     cp+=dp->rec_len;
     dp = (DIR *)cp;
   }
   
   rpwd(pip);
   strcat(pwdline,"/");
   strcat(pwdline,myname);
   return;
   
}
*/
#include "type.h"
#include "ucode.c"


int main(int argc, char *argv[]){
  pwd();

  return 0;
}