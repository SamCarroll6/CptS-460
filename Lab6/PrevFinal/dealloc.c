#include "header.h"


int incfreeinodes(int dev)
{   
    char buf[BLKSIZE];

    // dec free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count++;
    put_block(dev, 1, buf);
    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count++;
    put_block(dev, 2, buf);
}

int incfreeblocks(int dev)
{   
    char buf[BLKSIZE];

  // dec free inodes count in SUPER and GD
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_blocks_count++;
  put_block(dev, 1, buf);
  get_block(dev, 2, buf);
  gp = (GD *)buf;
  gp->bg_free_blocks_count++;
  put_block(dev, 2, buf);
}

int idalloc(int dev, int ino)
{
    char buf[BLKSIZE];

    if(ino > ninodes)
    {
        printf("ino %d out of range\n", ino);
        return -1;
    }
    get_block(dev, imap, buf);
    clr_bit(buf, ino - 1);

    put_block(dev, imap, buf);

    incfreeinodes(dev);
}

int bdalloc(int dev, int bno)
{
    char buf[BLKSIZE];

    if(bno > nblocks)
    {
        printf("block %d out of range\n", bno);
        return -1;
    }
    get_block(dev, bmap, buf);
    clr_bit(buf, bno - 1);

    put_block(dev, bmap, buf);

    incfreeblocks(dev);
}