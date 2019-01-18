#include "ext2.h"

main()
{
  u16 i, iblk, offset, blk;
  u32 *up;
  char c, temp[64], buf3[BLK];

  char file1[10], file2[10];

  strcpy(file1, "boot");
  strcpy(file2, "mtx");
  // Read block 2 to get the inode starting block
  getblk(2, buf1);

  // 1. WRITE YOUR CODE to get iblk = bg_inode_table block number

  gp = (GD *)buf1;
  iblk = gp->bg_inode_table;

  // 2. WRITE YOUR CODE to get root inode

  // Offet by 1 to get root
  getblk(iblk, buf1);
  ip = (INODE *)buf1 + 1;

  // Find /boot in root
  i = search(ip, file1);
  //getc();
  blk = (i / 8) + iblk;
  offset = i % 8;
  getblk(blk, buf1);
  ip = (INODE *)buf1 + offset;

  i = search(ip, file2);
  //getc();
  blk = (i / 8) + iblk;
  offset = i % 8;
  getblk(blk, buf1);
  ip = (INODE *)buf1 + offset;

  // Write YOUR C code to get the INODE of /boot/mtx
  // INODE *ip --> INODE
  //  if INODE has indirect blocks: get i_block[12] int buf2[  ]
  getblk((u16)ip->i_block[12], buf3);

  setes(0x1000); // MTX loading segment = 0x1000

  // load 12 DIRECT blocks of INODE into memory
  for (i = 0; i < 12; i++)
  {
    getblk((u16)ip->i_block[i], 0);
    putc('*');
    inces();
    getc();
  }

  if (ip->i_block[12])
  {
    up = (u32 *)buf3;
    // load INDIRECT blocks, if any, into memory
    while (*up)
    {
      getblk((u16)*up, 0);
      putc('.');
      inces();
      up++;
    }
  }

  prints("go?");
  getc();
}
