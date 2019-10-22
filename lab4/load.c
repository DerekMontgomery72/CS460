
#define TRK 18
#define CYL 36
#define BLK 1024

typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;



DIR *dp;
char buf1[BLK], buf2[BLK];
char *name[2];

int load(char *filename, PROC *p)
{
  //filename in /bin directory
  //load to p->pgdir[2048] &0xFFF00000
  //return 1 for success
  int *ptable = p->pgdir;
   p->pgdir = (int *)(0xFFF00000 + (p->pid - 1)*0x4000);
  
  name[0] = "bin";
  name[1] = filename;

  getblk(2,buf1);
  gp = (GD *)buf1;
  iblk = (u16)gp->bg_inode_table;

  getblk(iblk, buf1);
  for(i=0; i<2; i++){
    ino = FindDir(ip,name[i]);
    ino--;
    if(ino < 0)
      error();
    getblk(iblk + (ino/8),buf1);
    ip = (INODE *)buf1 + (ino % 8);
  }
  if((u16)ip->i_block[12])
    getblk((u16)ip->i_block[12],buf2);

  for(i=0; i<12; i++)
    {
      getblk((u16)ip->i_block[i],p->pgdir);
      p->pgdir++;
     
    }

  if((u16)ip->i_block[12])
    {
      up = (u32 *)buf2;
      while(*up){
	getblk(*up,p->pgdir);
	p->pgdir++ blk;
  
  // stuff from booter
  /*
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
  */
  
}

int FindDir(INODE *ip, char *name)
{
  int i = 0; char c;
  for(i=0; i<12; i++)
    {
      if((u16)ip->i_block[i])
	{
	  getblk(((u16)ip->i_block[i]),buf2);
	  while((char *)dp < (buf2 + BLK))
	    {
	      c = dp->name[dp->name_len];
	      dp->name[dp->name_len] = 0;
	      printf("%s\n",dp->name);

	      if(strcmp(name,dp->name) == 0)
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


int getblk(u16 blk, char *buf)
{
  readfd((2*blk)/CYL,((2*blk)%CYL)TRK,((2*blk)%CYL)%TRK,buf);
}
