#include "header.h"

int checkints(char *s)
{
    int i = 0, len;
    len = strlen(s);
    for(i = 0; i < len; i++)
    {
        if(s[i] >= '0' && s[i] <= '9')
        {
            return 1;
        }
    }
    return 0;
}

int my_read()
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
    if(paths[1] == NULL)
    {
        paths[1] = readcustominput("Enter bytes to read : ");
    }
    if(checkints(pass))
    {
        fd = atoi(pass);
    }
    else
    {
        printf("Error : Invalid FD value\n");
        return 0;
    }
    if(checkints(paths[1]))
    {
        bytes = atoi(paths[1]);
    }
    else
    {
        printf("Error : Invalid bytes value\n");
        return 0;
    }
    buf = (char*)(malloc(sizeof(char) * bytes));
    read_file(fd, buf, bytes);
}

int read_file(int fd, char buf[], int bytes)
{
    OFT *check;
    MINODE *mip;
    INODE *pip;
    int avail, size, offset, lbk, start, remain, block, count = 0, i = 0, ii = 0, ii2 = 0, block2, block3;
    char readbuf[BLKSIZE];
    int buf2[BLKSIZE], buf3[BLKSIZE];
    char *cq = buf;
    check = running->fd[fd];
    if(check == 0)
    {
        printf("Error : Invalid FD value, no open file found\n");
        return 0;
    }
    if(check->mode != R && check->mode != WR)
    {
        printf("Error : FD not opened for read\n");
        return 0;
    }
    mip = check->mptr;
    pip = &mip->INODE;
    offset = check->offset;
    size = pip->i_size;

    // bytes available for read
    avail = size - offset;

    while(bytes > 0 && avail > -(bytes))
    {
        // Logical block
        lbk = offset / BLKSIZE;
        // byte to start read in logical block
        start = offset % BLKSIZE;

        if(lbk < 12)
        {
            block = pip->i_block[lbk];
            get_block(mip->dev, block, readbuf);
        }
        else if(lbk >= 12 && lbk < 256 + 12)
        {
            block2 = pip->i_block[12];
            get_block(mip->dev, block2, buf2);
            get_block(mip->dev, buf2[i], readbuf);
            i++;
        }
        else
        {
            block = pip->i_block[13];
            get_block(mip->dev, block, buf2);
            get_block(mip->dev, buf2[ii], buf3);
            get_block(mip->dev, buf3[ii2], readbuf);
            ii2++;
            if(ii2 + 1 == 256)
            {
                ii2 = 0;
                ii++;
            }
        }
        char *cp = readbuf + start;
        // number of bytes remaining in logical block
        remain = BLKSIZE - start;
        int cpy;
        cpy = (bytes < remain) ? bytes : remain;

        memcpy(cq, cp, cpy);
        cq = cq + cpy;
        check->offset += cpy;
        offset = check->offset;
        count += cpy;
        avail -= cpy;
        bytes -= cpy;
        // while(remain > 0)
        // {
        //     *cq++ = *cp++;
        //     check->offset++;
        //     offset = check->offset;
        //     count++;
        //     avail--;
        //     bytes--;
        //     remain--;
        //     if(bytes <= 0 || avail <= 0)
        //     {
        //         break;
        //     }
        // }
    }
    // printf("Read : read %d char from file %d\n", count, fd);
    return count;
}  

int my_cat()
{
    int FD;
    char *pass;
    if(name[0] == NULL)
    {
        char *loopread;
        printf("----------------------\n");
        printf("Reading CAT from stdin\n");
        printf("----------------------\n");
        while(1)
        {
            loopread = readcustominput("");
            printf("%s\n", loopread);
        }
        return 0;
    }
    pass = (char*)(malloc(sizeof(char) * strlen(pathname)));
    strcpy(pass,pathname);
    FD = open_file(pass, "R");
    if(FD == -1)
    {
        printf("Error : File could not be opened\n");
        return 0;
    }
    int n, i;
    char mybuf[BLKSIZE];
    while(n = read_file(FD, mybuf, BLKSIZE))
    {
        mybuf[n] = 0;
        for(i = 0; i < BLKSIZE; i++)
        {
            if(mybuf[i] == '\\')
            {
                if(mybuf[i+1] == 'n')
                {
                    putchar('\n');
                    i++;
                }
                else
                {
                    putchar(mybuf[i]);
                }
            }
            else
            {
                putchar(mybuf[i]);               
            }
        }
    }
    close_file(FD);
}