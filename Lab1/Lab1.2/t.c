/*******************************************************
*                      t.c file                        *
*******************************************************/
#include "ext2.h"

main()
{ 
  u16    i, iblk, count, size;
  char   c, temp[64];

// Read block 2 to get the inode starting block
  prints("read block# 2 (GD)\n\r");
  prints("this is sams \n\r");
  getblk(2, buf1);

// 1. WRITE YOUR CODE to get iblk = bg_inode_table block number

  gp = (GD*)buf1;
  iblk = (u16)gp->bg_inode_table;

  prints("inode_block="); putc(iblk+'0'); prints("\n\r"); 

// 2. WRITE YOUR CODE to get root inode
  prints("read inodes begin block to get root inode\n\r");

// Offet by 1 to get root
  getblk(iblk, buf1);
  ip = (INODE*)buf1 + 1;


// 3. WRITE YOUR CODE to step through the data block of root inode
   prints("read data block of root DIR\n\n\r");

// Step through the iblocks 0-11 (the direct iblocks)
   for(i = 0; i < 12; i++)
   {
       if(ip->i_block[i] == 0)
       {
           break;
       }
// Get block for each block in given inode
       getblk((u16)ip->i_block[i], buf2);
       dp = (DIR*)buf2;

// 4. print file names in the root directory /
       while(count < BLK && dp->inode)
       {
           memcpy(temp, dp->name, dp->name_len);
           temp[dp->name_len] = '\0';
           prints(temp);
           prints("  ");
           count += dp->rec_len;
           dp = (DIR*)((char*)dp + dp->rec_len);
       }
   }
    prints("\n\n\rAll Done");
}  
