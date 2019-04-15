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
    int lbk, startByte, remain, offset, block, count = 0;
    MINODE *mip;
    INODE *pip;
    check = running->fd[fd];
    mip = check->mptr;
    pip = &mip->INODE;
    offset = check->offset;
    char wbuf[BLKSIZE], *cp, *cq;

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
        printf("%d\n", offset);
        count = 0;
        cq = buf + offset;
        lbk = offset / BLKSIZE;
        startByte = offset % BLKSIZE;
        if(lbk < 12)
        {
            printf("%d\n", pip->i_block[lbk]);
            if(pip->i_block[lbk] == 0)
            {
                pip->i_block[lbk] = balloc(mip->dev);
            }
            block = pip->i_block[lbk];
        }
        else if(lbk >= 12 && lbk < 256 + 12)
        {
            if(pip->i_block[12] == 0)
            {

            }
        }
        else
        {
            
        }
        get_block(mip->dev, block, wbuf);
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
        printf("%s\n", wbuf);
        put_block(mip->dev, block, wbuf);
    }
}