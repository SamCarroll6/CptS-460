#include "header.h"

int open_file()
{
    MINODE *pathfollow;
    if(name[0] == NULL)
    {
        
    }
    if(!strcmp(name[0], "/"))
        pathfollow = findval(root);
    else
    {
        pathfollow = findval(running->cwd);
    }
        
}