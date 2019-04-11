#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>

#include "type.h"

extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;

extern char gpath[256];
extern char *name[64]; // assume at most 64 components in pathnames
extern int  n;
extern int  fd, dev;
extern int  nblocks, ninodes, bmap, imap, inode_start;
extern char pathname[256], parameter[256];

int open_file()
{
    
}

int truncate(MINODE *mip)
{

}

int close_file(int fd)
{

}

// int lseek(int fd, int position)
// {

// }

int pfd()
{

}