



/************ t.c file ***************************
  Under Linux: cc t.c
  Run          a.out mtximage
**************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <ext2fs/ext2_fs.h>

// define shorter TYPES, save typing efforts
typedef struct ext2_group_desc  GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;    // need this for new version of e2fs

GD    *gp;
SUPER *sp;
INODE *ip;
DIR   *dp; 

char buf[1024];
int fd;
int iblock;

char *dev = "mtximage";

int search(INODE *pip, char *name)
{
  int    i, iblk;
  char   *c, temp[64], buf2[1024];
// Step through the iblocks 0-11 (the direct iblocks)
   for(i = 0; i < 12; i++)
   {
       if(pip->i_block[i] == 0)
       {
           break;
       }
	   
// Get block for each block in given inode
       getblk(fd, pip->i_block[i], buf2);
	   c = buf2;
       dp = (DIR*)buf2;
// Search block for name match
       while(c < &buf2[1024])
       {
           memcpy(temp, dp->name, dp->name_len);
           temp[dp->name_len] = '\0';
// If found return inode number
		   if(!strcmp(temp, name))
		   {
			  printf("match = %s\n", temp);
              printf("num = %d\n", dp->inode);
			  return (dp->inode - 1);
		   }
           c += dp->rec_len;
           dp = (DIR*)c;
       }
   }
// Return error if not found
   return -1;
}

int getblk(int fd, int blk, char *buf)
{
  lseek(fd, blk*1024, 0);
  read(fd, buf, 1024);
}

int show(char *dev)
{
  int i, blk, offset;
  char *cp;

  fd = open(dev, O_RDONLY);
  if (fd < 0){
    printf("open failed\n");
    exit(1);
  }

  getblk(fd, 1, buf);
  sp = (SUPER *)buf;
  printf("magic = 0x%4x\n", sp->s_magic);
  getchar();


  getblk(fd, 2, buf);
  gp = (GD *)buf;

  iblock = gp->bg_inode_table;
  printf("inode_block=%d\n", iblock);
  
  getblk(fd, iblock, buf);
  ip = (INODE *)buf + 1;

  i = search(ip, "boot");
  printf("i = %d\n", i);
  blk = (i / 8) + iblock;
  printf("blk = %d\n", blk);
  offset = i % 8;
  printf("offset = %d\n", offset);
  getblk(fd, blk, buf);
  ip = (INODE *)buf + offset;
  i = search(ip, "mtx");
    printf("i = %d\n", i);
  blk = (i / 8) + iblock;
  printf("blk = %d\n", blk);
  offset = i % 8;
  printf("offset = %d\n", offset);
  getblk(fd, blk, buf);
  ip = (INODE *)buf + offset;

  printf("mode=0x%4x ", ip->i_mode);
  printf("size=%d\n", ip->i_size);

  for (i=0; i<15; i++){
    if (ip->i_block[i])
       printf("i_block[%d]=%d\n", i, ip->i_block[i]);
  }

  getblk(fd, ip->i_block[0], buf);
  dp = (DIR *)buf;
  cp = buf;
  
  while(cp < buf + 1024){
     printf("%s ", dp->name);
     getchar();
     cp += dp->rec_len;
     dp = (DIR *)cp;
  }
}

int main(int argc, char *argv[ ])
{
  if (argc>1)
     dev = argv[1];
  show(dev);
}

