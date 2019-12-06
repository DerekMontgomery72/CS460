typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_dir_entry_2 DIR;

#define S_ISDIR(m)    ((m & 0170000) == 0040000)  //Directory
#define S_ISCHR(m)    ((m & 0170000) == 0020000)  //char specieal
#define S_ISBLK(m)    ((m & 0170000) == 0060000)  //block special
#define S_ISREG(m)    ((m & 0170000) == 0100000)  //regular file
#define S_ISFIFO(m)   ((m & 0170000) == 0010000)  //fifio
#define S_ISLNK(m)    ((m & 0170000) == 0120000)  //link


#define FREE   0
#define Ready  1

#define BLKSIZE 1024
#define NMINODE 64
#define NFD     8
#define NPROC   2

typedef struct ext2_inode{

u16 i_mode; /* FILE mode */
u16 i_uid; 
u32 i_size;
u32 i_atime;
u32 i_ctime;
u32 i_mtime;
u32 i_dtime;
u16 i_gid;
u16 i_link_count;
u32 i_blocks;
u32 i_flags;
u32 i_block[15];
u32 i_date;
u32 i_time;

}INODE;

typedef struct minode{
  INODE INODE;
  int dev, ino;
  int refCount;
  int dirty;
  int mounted;
  struct mntable *mptr;
}MINODE;

typedef struct oft{
  int mode;
  int refCount;
  MINODE *mptr;
  int offset;
}OFT;

typedef struct mystat{
  int st_dev, st_ino, st_nlink;
  int st_size;
  int atime, mtime, ctime, st_blksize, st_blocks;
  u16 st_mode;
  u16 st_uid;
  u16 st_gid;
}MYSTAT;

typedef struct dirent{
  u32  d_ino;    //file serial number
  char d_name[]; //name of entry
}DIREENT;


typedef struct pipe{
  char buf[BLKSIZE];    // circular data buffer
  int  head, tail;    // circular data index
  int  data, room;    // number of data & room in pipe
  int  status;        // FREE or  BUSY

}PIPE;
