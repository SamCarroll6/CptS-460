#ifndef _HEADER_H_
#define _HEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>


/*************** type.h file *********************************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;   

#define BLKSIZE  1024

#define NMINODE    64
#define NOFT       64
#define NFD        10
#define NMOUNT      4
#define NPROC       2
#define R           0
#define W           1
#define WR          2
#define APPEND      3

typedef struct minode{
  INODE INODE;
  int dev, ino;
  int refCount;
  int dirty;
  int mounted;
  struct mntable *mptr;
}MINODE;

typedef struct oft{
  int  mode;
  int  refCount;
  MINODE *mptr;
  int  offset;
}OFT;

typedef struct proc{
  struct proc *next;
  int          pid;
  int          uid, gid;
  MINODE      *cwd;
  OFT         *fd[NFD];
}PROC;

MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;

char gpath[256];
char *name[64], *paths[64]; // assume at most 64 components in pathnames
int  n;

int  fd, dev;
int  nblocks, ninodes, bmap, imap, inode_start;
char line[256], cmd[32], pathname[256];

// Utility/Mount Functions 
int get_block(int mdev, int blk, char buf[]);
int put_block(int mdev, int blk, char buf[]);
int init(void);
int mountroot(char *diskname);
MINODE *iget(int mdev, int ino);
int iput(MINODE *mip);
int getino(MINODE *mip, char *name2);
INODE *get_inode(int mdev, int ino);
char* Parse(char* input);
char* readinput(void);
int tokenize(char *pathname);
int search(INODE *pip, char *name);
int is_empty(MINODE *mip);
void menu(void);
int getarrayval(char *input);
char* readcustominput(char *statement);

// PWD, CD, LS functions
void lsdo(void);
void ls_dir(MINODE *mip);
void ls_file(MINODE* mip, char *name2);
MINODE* findval(MINODE *mip);
int checktype(MINODE *mip);
int chdir(void);
int findmyname(MINODE *parent, u32 myino, char **myname);
int findino(MINODE *mip, u32 myino);
void pwd(void);
void rpwd(MINODE *pr);
int quit(void);

// Allocating functions and helpers
int tst_bit(char *buf, int bit);
int set_bit(char *buf, int bit);
int clr_bit(char *buf, int bit);
int decFreeInodes(int dev);
int decFreeClocks(int dev);
int ialloc(int dev);
int balloc(int dev);

// Deallocating functions and helpers
int incfreeinodes(int dev);
int incfreeblocks(int dev);
int idalloc(int dev, int ino);
int bdalloc(int dev, int bno);
int bdallocindirects(int dev, int bno);

// MKDIR
int mdir(void);
int smkdir(MINODE *mip, char *bname);
int enter_child(mip, ino, bname);

// Creat
int creat_file(void);
int mycreat(MINODE *mip, char *bname);

// RMDIR
int rm_dir(void);
int rm_child(MINODE *pmip, char *name);

// SYMLINK, LINK, UNLINK
int mysymlink(void);
char* myreadlink(void);
int link(void);
int myunlink(void);

// Touch and CHMOD
int touch(void);
int mychmod(void);

// Open, Close, lseek
int open_file();
int preopen();
int checkUmode(char *Umode, MINODE *pathtofollow);
int pfd(void);
int close_file(int fd);
int pre_close();

// Read, Cat
int checkints(char *s);
int getIndirects(int device, int bno);
int my_read();
int read_file(int fd, char buf[], int bytes);

// Write, Cp


#endif