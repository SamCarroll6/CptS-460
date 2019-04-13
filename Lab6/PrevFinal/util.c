#include "header.h"

int get_block(int mdev, int blk, char buf[])
{
    lseek(mdev, (long)blk*BLKSIZE, 0);
    read(mdev, buf, BLKSIZE);
    return 1;
}

int put_block(int mdev, int blk, char buf[])
{
    lseek(mdev, (long)blk*BLKSIZE, 0);
    write(mdev, buf, BLKSIZE);
    return 1;
}

int mountroot(char *diskname)
{
    printf("mountroot()\n");
    char buf[BLKSIZE];
    get_block(fd, 1, buf);
    sp = (SUPER*)buf;
    if(sp->s_magic != 0xEF53)
    {
        printf("Magic = %x is not an ext2 filesystem\n", sp->s_magic);
        exit(1);
    }
    nblocks = sp->s_blocks_count;
    ninodes = sp->s_inodes_count;
    get_block(fd, 2, buf);
    gp = (GD*)buf;
    bmap = gp->bg_block_bitmap; 
    imap = gp->bg_inode_bitmap; 
    inode_start = gp->bg_inode_table;
    root = iget(fd, 2);
    proc[0].cwd = iget(fd, 2);
    proc[1].cwd = iget(fd, 2);
    running = &proc[0];
}

int init(void)
{
    printf("Checking EXT2 FS... init()\n");
    int i = 0;
    int count = 0;
    for(i = 0; i < NPROC; i++)
    {
        proc[i].pid = i;
        proc[i].uid = i;
        for(count = 0; count < NFD; count++)
        {
            proc[i].fd[count] = 0;
        }
        proc[i].next = &proc[i+1];
    }
    proc[NPROC - 1].next = &proc[0];
    for(i; i < NMINODE; i++)
    {
        minode[i].refCount = 0;
    }
    running = &proc[0];
    root = NULL;
}

MINODE *iget(int mdev, int ino)
{
    MINODE *mip;
    int i = 0;
    for(i; i < NMINODE; i++)
    {
        mip = &minode[i];
        if((mip->refCount) && (mip->dev == mdev) && (mip->ino == ino))
        {
            mip->refCount++;
            return mip;
        }
    }
    for(i = 0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if(mip->refCount == 0)
        {
            mip->dev = mdev;
            mip->ino = ino;
            mip->refCount++;
            mip->dirty = 0;
            mip->mounted = 0;
            mip->INODE = *get_inode(mdev, ino);
            return mip;
        }
    }
    printf("Error: No MINODES\n");
    return 0;
}

int iput(MINODE *mip)
{
    char buf[BLKSIZE];
    INODE *pip;
    if(mip == NULL)
        return 0;
    mip->refCount--;
    // If the ref count is greater than 0 
    // and it's clean we can return.
    if(mip->refCount > 0 && !mip->dirty)
        return 0;
    int ino = mip->ino;
    // 8 is the number of inodes per block
    // find which disk block, and which inode 
    // in that block
    int blk = (ino - 1) / 8 + inode_start;
    int offset = (ino - 1) % 8;
    // get the disk block
    get_block(mip->dev, blk, buf);
    // use offset to get the inode spot
    pip = (INODE*)buf + offset;
    // copy the inode from mip into this spot
    *pip = mip->INODE;
    // write this back to the disk
    put_block(mip->dev, blk, buf);
    mip->dirty = 0;
    return 0;
}

int getino(MINODE *mip, char *name2)
{
    int i, ino;
    INODE *check = &mip->INODE;
    ino = search(check, name2);
    if(ino == -1)
    {
        printf("Name %s not found\n", name2);
        return -1;
    }
    return ino;
}

INODE *get_inode(int mdev, int ino)
{
    char buf[BLKSIZE];
    int blk, offset;
    get_block(mdev, inode_start, buf);
    ip = (INODE*)buf + 1;
    blk = (ino - 1) / 8 + inode_start;
    offset = (ino - 1) % 8;
    get_block(mdev, blk, buf);
    ip = (INODE*)buf + offset;
    return ip;
}

char* readinput(void)
{
    char *token;
    int i = 1, reset = 0;
    char line[1024];
    char *linecpy;
    printf("input command : ");
    fgets(line, sizeof(line), stdin);
    reset = strlen(line);
    linecpy = (char*)malloc(sizeof(char) * reset);
    strcpy(linecpy, line);
    strtok(linecpy, "\n");
    return linecpy;
}

char* Parse(char* input)
{
    int len, i = 0;
    char *token, *hold;
    // Parse function by spaces
    // Store each input in variable paths[i]
    token = strtok(input, " ");
    while(token = strtok(NULL, " "))
    {
        len = strlen(token);
        hold = (char*)malloc(len * sizeof(char));
        strcpy(hold, token);
        paths[i] = hold;
        i++;
    }
    return (i > 0) ? paths[0] : NULL;
}

int tokenize(char *pathname)
{
    int i = 0;
    while(name[i])
    {
        name[i] = NULL;
        i++;
    }
    if(pathname == NULL)
        return 0;
    char *token;
    i = 0;
    if(pathname[0] == '/')
    {
        name[i] = "/";
        i++;
    }
    token = strtok(pathname, "/");
    name[i] = token;
    i++;
    while(token)
    {
        token = strtok(NULL, "/");
        if(token)
        {
            name[i] = token;
            i++;
        }
    }
    return i;
}

/*
    Helper function, just returns if a file 
    type is directory or standard file.
*/
int checktype(MINODE *mip)
{
    if(!mip)
        return 0;
    INODE* pip = &mip->INODE;
    return S_ISDIR(pip->i_mode);
}

int search(INODE *pip, char *name)
{
  int size, i = 0;
  char buf[BLKSIZE];
  char *cp;
  size = pip->i_size;
  for(i; i < 12; i++)
  {
    if(pip->i_block[i] == 0)
        return -1;
    get_block(fd, pip->i_block[i], buf);

    dp = (DIR *)buf;
    cp = buf;
    char nameval[BLKSIZE + 1];
    while(cp < &buf[BLKSIZE])
    {
        memcpy(nameval, dp->name, dp->name_len);
        nameval[dp->name_len] = '\0';
	    if(!strcmp(nameval, name))
	    {
		    return dp->inode;
	    }
        cp += dp->rec_len;
        dp = (DIR*)cp;
    }
  }
  return -1;
}

/*
    Follow tokenized pathname using search function.
    If path exists you'll have the ino of the directory
    returned to you, turn this to MINODE and return.
    If directory doesn't exist return NULL. 
*/
MINODE* findval(MINODE *mip)
{
    int i, ino;
    INODE *check = &mip->INODE;
    if(name[0] == NULL)
        return mip;
    if(!strcmp(name[0],"/"))
        i = 1;
    else
    {
        i = 0;
    }
    while(name[i])
    {
        ino = search(check, name[i]);
        if(ino == -1)
        {
            printf("Name %s does not exist\n", name[i]);
            return NULL;
        }
        mip = iget(mip->dev, ino);
        check = &mip->INODE;
        i++;
    }
    return mip;
}

// Check if directory is empty.
int is_empty(MINODE *mip)
{   
    INODE *pip = &mip->INODE;
    int size, i = 0;
    char buf[BLKSIZE];
    char *cp;
    size = pip->i_size;
    // Only needs to check first direct block
    // That's where first additional file would be.
    get_block(fd, pip->i_block[0], buf);

    dp = (DIR *)buf;
    cp = buf;
    char nameval[BLKSIZE + 1];
    while(cp < &buf[BLKSIZE])
    {
        memcpy(nameval, dp->name, dp->name_len);
        nameval[dp->name_len] = '\0';
        cp += dp->rec_len;
        dp = (DIR*)cp;
        i++;
    }
    return (i == 2) ? 1 : 0;
}

void menu(void)
{
    printf("==================== MENU ====================\n");
	printf("mkdir  rmdir  ls  cd  pwd  creat  touch  pfd\nrm  link  unlink  symlink  chmod  quit  open\n");
    printf("==============================================\n");
}

int getarrayval(char *input)
{
    int i = 0;
    char *pars[15] = {"ls", "cd", "pwd", "quit", "mkdir", "creat", "rmdir", "symlink", "link", "touch", "unlink", "chmod", "menu", "open", "pfd"};
    if(!strcmp(input, "rm"))
        return 10;
    if(!strcmp(input, "help") || !strcmp(input, "?"))
        return 12;
    for(i; i < 15; i++)
    {
        if(!strcmp(input, pars[i]))
            return i;
    }
    return -1;
}

char* readcustominput(char *statement)
{
    char *token;
    int i = 1, reset = 0;
    char line[1024];
    char *linecpy;
    printf("%s", statement);
    fgets(line, sizeof(line), stdin);
    reset = strlen(line);
    linecpy = (char*)malloc(sizeof(char) * reset);
    strcpy(linecpy, line);
    strtok(linecpy, "\n");
    return linecpy;
}