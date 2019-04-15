#include "header.h"

int write_file()
{
    int val = 0, fd, bytes;
    char *hold =  (char*)(malloc(sizeof(char) * strlen(pathname)));
    char *buf;
    strcpy(hold, pathname);
    char *pass = &hold[0];
    if(paths[0] == NULL)
    {
        pass = readcustominput("Enter fd value : ");
    }
    buf = readcustominput("Enter string to write : ");
    if(checkints(pass))
    {
        fd = atoi(pass);
    }
    else
    {
        printf("Error : Invalid FD value\n");
        return 0;
    }
    bytes = strlen(buf);
    mywrite(fd, buf, bytes);
}

int mywrite(int fd, char buf[], int nbytes)
{
    OFT *check;
    int lbk, startByte, remain, offset, block, count = 0, indir = 0;
    MINODE *mip;
    INODE *pip;
    check = running->fd[fd];
    mip = check->mptr;
    pip = &mip->INODE;
    offset = check->offset;
    int test[BLKSIZE], t2[BLKSIZE];
    char wbuf[BLKSIZE], *cp, *cq, blank[BLKSIZE];

    if(check == 0)
    {
        printf("Error : Invalid FD value, no open file found\n");
        return 0;
    }
    if(check->mode != W && check->mode != WR && check->mode != APPEND)
    {
        printf("Error : FD not opened for write\n");
        return 0;
    }

    while(nbytes > 0)
    {
        count = offset;
        //cq = buf + offset;
        cq = buf;
        lbk = offset / BLKSIZE;
        startByte = offset % BLKSIZE;
        if(lbk < 12)
        {
            if(pip->i_block[lbk] == 0)
            {
                pip->i_block[lbk] = balloc(mip->dev);
            }
            block = pip->i_block[lbk];
            get_block(mip->dev, block, wbuf);
        }
        else if(lbk >= 12 && lbk < 256 + 12)
        {
            if(pip->i_block[12] == 0)
            {
                pip->i_block[12] = balloc(mip->dev);
                put_block(mip->dev, pip->i_block[12], t2);
            }
            get_block(mip->dev, pip->i_block[12], test);
            if(test[indir] == 0)
            {
                test[indir] = balloc(mip->dev);
                put_block(mip->dev, test[indir], blank);
            }
            get_block(mip->dev, test[indir], wbuf);
            block = test[indir];
            printf("%d\n", block);
            indir++;
        }
        else
        {
            
        }
        cp = wbuf + startByte;
        remain = BLKSIZE - startByte;
        while(remain > 0)
        {
            *cp++ = *cq++;
            nbytes--;
            remain--;
            offset++;
            count++;
            check->offset++;
            if(offset > pip->i_size)
            {
                pip->i_size++;
            }
            if(nbytes <= 0)
                break;
        }
        while(count < BLKSIZE)
        {
            *cp = '\0';
            *cp++;
            count++;
        }
        put_block(mip->dev, block, wbuf);
    }

}

int my_cp()
{
    int fd, gd, n;
    char buf[BLKSIZE];
    if(paths[0] == NULL || paths[1] == NULL)
    {
        printf("Error : Invalid src or dest pathname\n");
        return 0;
    }
    char *pass = (char*)(malloc(sizeof(char) * strlen(pathname)));
    char *pass2 = (char*)(malloc(sizeof(char) * strlen(paths[1])));
    strcpy(pass, pathname);
    fd = open_file(pass, "R");
    strcpy(pass2, paths[1]);
    gd = open_file(pass2, "W");
    while(n = read_file(fd, buf, BLKSIZE))
    {
        mywrite(gd, buf, n);
    }
    running->fd[gd]->mptr->INODE.i_size = running->fd[fd]->mptr->INODE.i_size;
    running->fd[gd]->offset = running->fd[fd]->offset;
    close_file(fd);
    close_file(gd);
}