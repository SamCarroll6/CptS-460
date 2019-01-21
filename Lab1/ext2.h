#ifndef EXT2_H
#define EXT2_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

#define TRK 18
#define CYL 36
#define BLK 1024

typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

GD    *gp;
INODE *ip;
DIR   *dp;

char buf1[BLK], buf2[BLK];
u16 color = 0x0A;
u8 ino;
u16 NSEC = 2;

struct ext2_inode {
	u16	i_mode;		/* File mode */
	u16	i_uid;		/* Owner Uid */
	u32	i_size;		/* Size in bytes */
	u32	i_atime;	/* Access time */
	u32	i_ctime;	/* Creation time */
	u32	i_mtime;	/* Modification time */
	u32	i_dtime;	/* Deletion Time */
	u16	i_gid;		/* Group Id */
	u16	i_links_count;	/* Links count */
	u32	i_blocks;	/* Blocks count */
	u32	i_flags;	/* File flags */
    
	u32     dummy;
	u32	i_block[15];    /* Pointers to blocks */
    u32     pad[7];         /* inode size MUST be 128 bytes */
};

struct ext2_group_desc
{
	u32	bg_block_bitmap;		/* Blocks bitmap block */
	u32	bg_inode_bitmap;		/* Inodes bitmap block */
	u32	bg_inode_table;		/* Inodes table block */
	u16	bg_free_blocks_count;	/* Free blocks count */
	u16	bg_free_inodes_count;	/* Free inodes count */
	u16	bg_used_dirs_count;	/* Directories count */
	u16	bg_pad;
	u32	bg_reserved[3];
};

struct ext2_dir_entry_2 {
	u32	inode;			/* Inode number */
	u16	rec_len;		/* Directory entry length */
	u8	name_len;		/* Name length */
	u8	file_type;
	char	name[255];      	/* File name */
};

u16 prints(char *s)
{
    while(*s)
    {
        putc(*s++);
    }
}

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
}

// Will modify code from Lab1.2 to find given name
u16 search(INODE *pip, char *name)
{
  u16    i, iblk;
  char   *c, temp[64];
// Step through the iblocks 0-11 (the direct iblocks)
   for(i = 0; i < 12; i++)
   {
       if(pip->i_block[i] == 0)
       {
           break;
       }
	   
// Get block for each block in given inode
       getblk((u16)pip->i_block[i], buf2);
	   c = buf2;
       dp = (DIR*)buf2;
// Search block for name match
       while(c < &buf2[BLK])
       {
           memcpy(temp, dp->name, dp->name_len);
           temp[dp->name_len] = '\0';
// If found return inode number
		   if(!strcmp(temp, name))
		   {
			  return (u16)(dp->inode - 1);
		   }
           c += dp->rec_len;
           dp = (DIR*)c;
       }
   }
// Return error if not found
   error();
}

#endif