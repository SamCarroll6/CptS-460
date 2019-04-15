#include "header.h"

int mytruncate(MINODE *mip, int mode)
{
    // nblocks, BLKSIZE
    int i;
    INODE *pip = &mip->INODE;
    int device = mip->dev;
    int buf[BLKSIZE];
    for(i = 0; i < 12 && pip->i_block[i] != 0; i++)
    {
        if(pip->i_block[i] == 0)
        {
            break;
        }
        bdalloc(device, pip->i_block[i]);
    }
    if(pip->i_block[12] != 0)
    {
        bdallocindirects(device, pip->i_block[12]);
    }
    if(pip->i_block[13] != 0)
    {
        get_block(device, pip->i_block[13], buf);
        for(i = 0; i < 256; i++)
        {
            bdallocindirects(device, buf[i]);
        }
    }
    pip->i_mtime = time(NULL);
    pip->i_atime = time(NULL);
    pip->i_blocks = 0;
    pip->i_size = 0;
}

int checkUmode(char *Umode, MINODE *pathfollow)
{
    INODE* pip = &pathfollow->INODE;
    u16 mode = pip->i_mode;
    char *test;
    int length = strlen(Umode);
    test = (char*)malloc(sizeof(char) * length);
    strcpy(test, Umode);
    if(strcmp(test, "0") == 0 || strcmp(test, "R") == 0)
    {
        free(test);
        return (mode & S_IRUSR) ? 1 : 0;
    }
    else if(strcmp(test, "1") == 0 || strcmp(test, "W") == 0)
    {
        free(test);
        return (mode & S_IWUSR) ? 2 : 0;
    }
    else if(strcmp(test, "2") == 0 || strcmp(test, "RW") == 0)
    {
        free(test);
        return ((mode & S_IWUSR) && (mode & S_IRUSR)) ? 3 : 0;
    }
    else if(strcmp(test, "3") == 0 || strcmp(test, "APPEND") == 0)
    {
        free(test);
        return (mode & S_IWUSR) ? 4 : 0;
    }
    return 0;
}

int preopen()
{
    int val = 0;
    char *hold =  (char*)(malloc(sizeof(char) * strlen(pathname)));
    strcpy(hold, pathname);
    char *pass = &hold[0];
    if(paths[0] == NULL)
    {
        pass = readcustominput("Enter Pathname : ");
    }
    if(paths[1] == NULL)
    {
        paths[1] = readcustominput("Enter Mode to Open : ");
    }
    open_file(pass, paths[1]);
}

int open_file(char *p1, char *p2)
{
    char *hold1 = NULL;
    char *hold2 = NULL;
    MINODE *pathfollow;
    INODE *pip;
    OFT *new = (OFT*)(malloc(sizeof(OFT)));
    int val = 0, i = 0, hold = 1, empty = -1, check = 0;
    // if(paths[0] == NULL)
    // {
    //     paths[0] = readcustominput("Enter Pathname : ");
    //     strcpy(pathname, paths[0]);
    //     val = tokenize(paths[0]);
    // }
    // if(paths[1] == NULL)
    // {
    //     paths[1] = readcustominput("Enter Mode to Open : ");
    // }
    strcpy(pathname, p1);
    val = tokenize(p1);
    if(!strcmp(name[0], "/"))
        pathfollow = findval(root);
    else
    {
        pathfollow = findval(running->cwd);
    }
    if(!pathfollow)
    {
        creat_file();
        val = tokenize(pathname);
        if(!strcmp(name[0], "/"))
            pathfollow = findval(root);
        else
        {
            pathfollow = findval(running->cwd);
        }
    }
    int blah = 0;
    check = checktype(pathfollow);
    if(check == 0)
    {
        check = checkUmode(p2 , pathfollow);
        for(i = 0; i < 10; i++)
        {
            if(running->fd[i])
            {
                if(running->fd[i]->mptr->ino == pathfollow->ino && (check - 1) != 0)
                {
                    printf("Error : file already opened\n");
                    return -1;
                }
            }
            if(running->fd[i] == 0 && hold)
            {
                empty = i;
                hold = 0;
            }
        }   
        if(empty == -1)
        {
            printf("Error : No available FD's\n");
            return -1;
        }
        if(check)
        {
            if(check - 1 == APPEND)
            {
                new->offset = pathfollow->INODE.i_size;
            }
            else
            {
                new->offset = 0;
            }
            if(check - 1 == W)
            {
                mytruncate(pathfollow, check);
            }
            new->mode = check - 1;
            new->mptr = pathfollow;
            new->refCount++;
            running->fd[empty] = new;
            pathfollow->dirty = 1;
            return empty;
        }
        else
        {
            printf("Error : Open failed due to permissions\n");
            return -1;
        }
        
    }
    else
    {
        printf("File is of type directory and can't be opened\n");
        return -1;
    }
    return -1;
}

int pfd(void)
{
    int i;
    MINODE *mip;
    printf(" fd     mode    offset    inode\n");
    printf("----    ----    ------    -----\n");
    for(i = 0; i < NFD; i++)
    {
        if(running->fd[i] != 0)
        {
            printf("  %d     ", i);
            switch(running->fd[i]->mode)
            {
                case R:
                    printf("READ    ");
                    break;
                case W:
                    printf("WRITE   ");
                    break;
                case WR:
                    printf(" RW     ");
                    break;
                case APPEND:
                    printf("APPEND  ");
                    break;
                default:
                    printf("No Mode stored\n");
                    return 0;
            }
            printf("%6d", running->fd[i]->offset);
            printf("   ");
            mip = running->fd[i]->mptr;
            printf("[ %d , %d ]", mip->dev, mip->ino);
            putchar('\n');
        }
    }
}

int pre_close()
{
    int hold;
    if(paths[0])
    {
        hold = atoi(paths[0]);
        close_file(hold);
    }
    else
    {
        printf("Error : Invalid path, did not supply File Descriptor value\n");
        return 0;
    }
    
}

int close_file(int fd)
{
    OFT *old;
    MINODE *mip;
    if(fd >= NFD || fd < 0)
    {
        printf("Error : File Descriptor number is out of range\n");
        return 0;
    }
    old = running->fd[fd];
    if(old)
    {
        running->fd[fd] = 0;
        old->refCount--;
        if(old->refCount == 0)
        {
            iput(old->mptr);
            return 1;
        }
        return 1;
    }
    printf("Error : No matching File Descriptor found\n");
    return 1;
}