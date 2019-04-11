#include "header.h"

int mychmod(void)
{
    MINODE *mip;
    char *endptr;
    if(paths[0] && paths[1])
    {
        tokenize(paths[1]);
        if(name[0] == '/')
        {
            mip = findval(root);
        }
        else
        {
            mip = findval(running->cwd);
        }
        if(mip)
        {
            INODE *pip = &mip->INODE;
            // reset mode so it can be rewritten with or.
            pip->i_mode = pip->i_mode & 0xF000;
            // rewrite imode with or of new mode. 
            pip->i_mode |= strtol(pathname, &endptr, 8);
            mip->dirty = 1;
            iput(mip);
            return 1;
        }
        printf("Error: chmod file1 not found\n");
        return -1;
    }
    printf("Usage: chmod permissions file1\n");
    return -1;
}