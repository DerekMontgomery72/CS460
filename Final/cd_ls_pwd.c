
/**** globals defined in main.c file ****/
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



change_dir(char *pathname)
{
  int ino;
  MINODE *mip;
  
  if(strcmp(pathname, "") == 0){
    running->cwd = root;
  }
  ino = getino(pathname);
  mip=iget(dev,ino);
  
    if((mip->INODE.i_mode & 0xF000) == 0x4000){
      iput(running->cwd);
      running->cwd = mip;
    }
    else{
      iput(mip);
      printf("ERROR: NOT A DIR\n");
     }
  
}


int list_file(char *pathname)
{
 
  if(strcmp(pathname,"") != 0){
    if(pathname[0] == '/'){
      ls(pathname);
    }
    else{
      pwd(running->cwd);
      strcat(pwdline,"/");
      strcat(pwdline,pathname);
      ls(pwdline);
    }
  }
  else{
   pwd(running->cwd);
   ls(pwdline);
  }
}

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
       
   
 
//struct stat mystat, *sp;
char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";


int ls_file(int ino, char *name)
{
  
  MINODE *mip = iget(dev,ino);
  INODE *sp =&(mip->INODE);
  char *cp;
  DIR *dp;
  time_t mytime;
  int r, i;
  char buf[BLKSIZE],temp[256];
  char ftime[64];
  

  
//sp = &fstat;
//if ( (r = lstat(fname, &fstat)) < 0){
//printf("can’t stat %s\n", fname);
//exit(1);
//}
  
  if ((sp->i_mode & 0xF000) == 0x8000){ // if (S_ISREG())
    printf("%c",'-');
  }
 
  if ((sp->i_mode & 0xF000) == 0x4000){ // if (S_ISDIR())
   printf("%c",'d');
  }
  
  if ((sp->i_mode & 0xF000) == 0xA000){ // if (S_ISLNK())
   printf("%c",'l');
  }

  for (i=8; i >= 0; i--){
    //  printf("in for loop\n");
    if (sp->i_mode & (1 << i)){ // print r|w|x
      printf("%c", t1[i]);
    }
    else{
      printf("%c", t2[i]);
    }
// or print -
  }
 
  
  printf("%4d ",sp->i_links_count); // link count
  printf("%d ",sp->i_gid);
  // gid
   printf("%4d ",sp->i_uid);
  // uid
   printf("%8d ",sp->i_size);
  // file size
  // print time
   mytime = sp->i_mtime;
   strcpy(ftime, ctime(&mytime)); // print time in calendar form
  ftime[strlen(ftime)-1] = 0;
  // kill \n at end
  printf("%s ",ftime);
  // print name
 
  printf("%s",name); // print file basename
  // print -> linkname if symbolic file
  if ((sp->i_mode & 0xF000)== 0xA000){
      printf("\n");
      readlink(name);
  //printf(" -> %s", linkname); // print linked name
  }
  printf("\n");
  iput(mip);
  return 0;
}





int ls_dir(char *dname)
{
  
  DIR *dp;
  char buf[BLKSIZE];
  char name[64];
  int ino = getino(dname);
  if(ino == 0){
    printf("ERROR: could not find directory\n");
    return;
  }
  MINODE *mip = iget(dev,ino);
  int succ = get_block(dev,mip->INODE.i_block[0],buf);
  iput(mip);
  if(succ == -1){
    printf("ERROR: the program was unable to obtain the INODE.i_block[0]\n");
    exit(1);
  }
  printf("The block INODE.i_block[0] has been moved into the buffer\n");
  char *cp;
  cp = buf;
  dp = (DIR *)buf;
  while(cp  < (buf + BLKSIZE))
    {
      strncpy(name,dp->name,dp->name_len);
      name[dp->name_len] = 0;
      ls_file(dp->inode, name);
     
      cp+= dp->rec_len;
      dp = (DIR *)cp;
    }
 
// use opendir(), readdir(); then call ls_file(name)
}


int ls(char *argv)
{
 
 ls_dir(argv);

}


