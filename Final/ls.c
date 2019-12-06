// Globals Defined earlier. We need to figure out how to get these when in the kernel

/*
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC proc[NPROC], *running;
extern OFT NOFT[NFD],SNOFT[NFD];
extern char gpath[256];
extern char names[16][256];
extern int n;
extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;
extern char line[256],cmd[32],pathname[256],name[64];

#define OWNER 000700
#define GROUP 000070
#define OTHER 000007
*/


//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <sys/types.h>
//#include <time.h>
#include "type.h"
#include "ucode.c"
//#include <sys/stat.h>
//#include <dirent.h>


char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

struct stat mystat, *sp;

// Define my own get basename as the orignal was segfaulting
char *Get_base(char* fPath){
  char *cp, *bp, strTemp[256], *cp1;
  strcpy(strTemp,fPath);
  cp1 = strTemp;
  cp = mystrtok(&cp1, '/');
  while(cp != 0)
  {
    cp = mystrtok(&cp1, '/');
    if(cp == 0){
      break;
    }
    bp = cp;
  }

  return bp;
}

int ls_file(char *fname)
{
  struct stat fstat, *sp = &fstat;
  int r, i;
  char sbuf[4096], *base;
  base = Get_base(fname);

  r = stat(base, sp);
  if(sp == 0){
    printf("Error in LSTAT %d",r);
    return 1;
  }
  if ((sp->st_mode & 0xF000) == 0x8000){ // if (S_ISREG())
    printf("%c%s",'-', "\0");
  }
 
  if ((sp->st_mode & 0xF000) == 0x4000){ // if (S_ISDIR())
   printf("%c%s",'d', "\0");
  }
  
  if ((sp->st_mode & 0xF000) == 0xA000){ // if (S_ISLNK())
   printf("%c%s",'l', "\0");
  }

  for (i=8; i >= 0; i--){
    //  printf("in for loop\n");
    if (sp->st_mode & (1 << i)){ // print r|w|x
      printf("%c%s", t1[i], "\0");
    }
    else{
      printf("%c%s", t2[i], "\0");
    }
// or print -
  }
  printf(" %c", '\0');

  printf("%d ", sp->st_nlink);
  printf("%d ", sp->st_uid);
  printf("%d ", sp->st_size);
  
  // strcpy(ftime, ctime(&sp->st_ctime));
  // ftime[strlen(ftime)-1] = 0;
  // printf("%s ", ftime);


  printf("%s ",base);
  if(S_ISLNK(sp->st_mode))
    {
      r = readlink(fname, sbuf);
      printf(" -> %s", sbuf);
    }
  printf("\n%c", '\0');
}

int ls_dir(char *dname)
{
  int dir = open(dname, O_RDONLY);

  char name[256], *cp, buf[1024], path[256];
  read(dir,buf,1024);
  if(strlen(buf) == 0){
    printf("Unable to read directory\n");
  }
  DIR *dp;
  char dirName[256];
  int ino = 0;

  // open DIR to read names
   cp = buf;

   dp = (DIR *)buf;

   while( cp < (BLKSIZE + buf)){

     if(dp->inode)
     {
       strcpy(path,dname);
       strncpy(name,dp->name,dp->name_len);
       name[dp->name_len] = 0;
       if(strcmp(name, ".") !=0 && strcmp(name,"..") != 0)
       {
         strcat(path,name);
         ls_file(path);
       }

     }
     cp+=dp->rec_len;
     dp = (DIR *)cp;
   }
  
  close(dir);
}

int main(int argc, char *argv[])
{
  struct stat mystat, *sp;
  int r;
  char *s;
  char filename[1024], cwd[1024];
  s = argv[1]; // ls [filename]
  if(argc == 1) //no parameter: ls CWD
    s = "./";
  sp = &mystat;
  
  if((r = stat(s,sp)) < 0)
    {
      // stat() syscall
      printf("Stat Failed\n");
      exit(1);
    }
  strcpy(filename,s);
  
  if(s[0] != '/'){
    
    getcwd(cwd);
    strcpy(filename, cwd);
    if(strcmp(s,"./") != 0){
      strcat(filename, "/");
      strcat(filename,s);
    }
    
  }
  if(S_ISDIR(sp->st_mode)){
    ls_dir(filename); //list DIR
  }
  else
    {
      ls_file(filename);
    }
}
