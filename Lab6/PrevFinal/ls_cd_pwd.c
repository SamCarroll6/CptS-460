#include "header.h"


/*
    Simply a middle ground that decides what should be passed
    on ls call.
*/
void lsdo(void)
{
    int i = 0;
    MINODE *pathfollow;
    // if no parameter passed run on cwd
    if(name[0] == NULL)
    {
        ls_dir(running->cwd);
    }
    // otherwise get MINODE from file name passed and run
    // ls_file or ls_dir depending on type of file found.
    else
    {
        if(!strcmp(name[0], "/"))
            pathfollow = findval(root);
        else
        {
            pathfollow = findval(running->cwd);
        }
        
        if(pathfollow != NULL)
        {
            if(checktype(pathfollow))
                ls_dir(pathfollow);
            else
            {
                while(name[i])
                {
                    i++;
                }
                ls_file(pathfollow, name[i - 1]);
            }
        }
    }
}

/*
    Traverses through our directory mip, each file contained within
    mip is passed to ls_file to print it's contents.
*/
void ls_dir(MINODE *mip)
{
    INODE *pip = &mip->INODE;
    int i = 0;
    int size, count = 0;
    int buf[BLKSIZE];
    size = pip->i_size;
    for(i; i < 12; i++)
    {
        if(pip->i_block[i] == 0)
        {
            break;
        }
        printf("i_block[%d] = %d\n", i, pip->i_block[i]);
        get_block(mip->dev, pip->i_block[i], buf);
        dp = (DIR *)buf;
        char nameval[BLKSIZE + 1];
        while(count < size && dp->inode)
        {
            memcpy(nameval, dp->name, dp->name_len);
            nameval[dp->name_len] = '\0';
            MINODE* next = iget(mip->dev, dp->inode);
            ls_file(next, nameval);
            count+=dp->rec_len;
            dp = (void *)dp + dp->rec_len;
        }
    }
}

/*
    Prints like ls -l in standard linux. This
    function handles printing the information for
    an individual file or directory that is passed 
    in via MINODE* mip.
*/
void ls_file(MINODE* mip, char *name2)
{
    INODE* pip = &mip->INODE;
    u16 mode = pip->i_mode;
    time_t val = pip->i_ctime;
    char *mtime = ctime(&val);
    mtime[strlen(mtime) - 1] = '\0';
    if(S_ISDIR(pip->i_mode))
        putchar('d');
    else if(S_ISREG(pip->i_mode))
        putchar('-');
    else if(S_ISLNK(pip->i_mode))
    {
        putchar('l');
        char buf[64];
        strcpy(buf, name2);
        strcat(buf, " -> ");
        strcat(buf, (char*)pip->i_block);
        name2 = &buf[0];
    }
    else
    {
        putchar('-');
    }
    printf( (mode & S_IRUSR) ? "r" : "-");
    printf( (mode & S_IWUSR) ? "w" : "-");
    printf( (mode & S_IXUSR) ? "x" : "-");
    printf( (mode & S_IRGRP) ? "r" : "-");
    printf( (mode & S_IWGRP) ? "w" : "-");
    printf( (mode & S_IXGRP) ? "x" : "-");
    printf( (mode & S_IROTH) ? "r" : "-");
    printf( (mode & S_IWOTH) ? "w" : "-");
    printf( (mode & S_IXOTH) ? "x" : "-");
    printf("%4d%4d%4d  %s%8d    %s\n", pip->i_links_count, pip->i_gid, pip->i_uid, mtime, pip->i_size, name2);
}


/*
    cd function is here. If no path cwd = root. Otherwise
    depending on the path search through the root or cwd
    using findval function to return the minode of the new
    directory. use iput to write current cwd back to disk, and 
    update cwd to newly found directory.
*/
int chdir(void)
{
    MINODE *pathfollow;
    if(name[0] == NULL)
    {
        iput(running->cwd);
        running->cwd = root;
    }
    else
    {
        if(!strcmp(name[0], "/"))
        {
            pathfollow = findval(root);
        }
        else
        {
            pathfollow = findval(running->cwd);
        }
        if(checktype(pathfollow))
        {
            iput(running->cwd);
            running->cwd = pathfollow;
        }
        else
        {
            int i = 0;
            while(name[i])
            {
                i++;
            }
            printf("Error: %s is not a directory\n", name[i - 1]);
        }
    }
}

/*
    A helper function for pwd, this uses a parent inode
    and current ino to match current ino with one in 
    the parents files in order to return a name for the given file.
*/
int findmyname(MINODE *parent, u32 myino, char **myname)
{
  INODE *pip = &parent->INODE;
  int size, count = 0;
  char buf[BLKSIZE];
  size = pip->i_size;
  get_block(fd, pip->i_block[0], buf);

  dp = (DIR *)buf;
  while(count < size && dp->inode)
  {
	if(dp->inode == myino)
	{
        *myname = (char*)malloc(sizeof(char) * (dp->name_len + 1));
        strncpy(*myname, dp->name, dp->name_len);
        (*myname)[dp->name_len] = '\0';
		return 0;
	}
        dp = (void *)dp + dp->rec_len;
        count+=dp->rec_len;
  }
  return 1;
}

/*
    find ino is a helper function for pwd,
    it simply returns the parent ino of the 
    given minode.
*/
int findino(MINODE *mip, u32 myino)
{
    return getino(mip, "..");
}

/*
    Simple function, either print / if cwd is root
    or call rpwd on cwd if not. 
*/
void pwd(void)
{
    MINODE *pr = running->cwd;
    if(pr == root)
    {
        printf("/\n");
    }
    else
    {
        rpwd(pr);
        putchar('\n');
    }
}

/*
    The recursive part of pwd. If the cwd is not 
    the root this is called to traverse and find 
    the path back to root using .. directory. Once root 
    is found we come back through the recursive calls and
    print the name along the way.
*/
void rpwd(MINODE *pr)
{
    MINODE *pip;
    char *name2;
    if(pr == root)
    {
        return;
    }
    int pino = findino(pr, pr->ino);
    pip = iget(pr->dev, pino);
    findmyname(pip, pr->ino, &name2);
    rpwd(pip);
    printf("/%s", name2);
}

/*
    Pretty simply this is the quit function, this ensures
    that all minodes have been written to disk and then exits 
    the program.
*/
int quit(void)
{
    int i;
    MINODE *mip;
    for(i = 0; i < NMINODE; i++)
    {
        mip = &minode[i];
        if(mip->refCount > 0 && mip->dirty)
            iput(mip);
    }
    exit(0);
}