#include "header.h"

int checkUmode(char *Umode, MINODE *pathfollow)
{
    printf("%s\n", Umode);
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
}

int open_file()
{
    char *hold1 = NULL;
    char *hold2 = NULL;
    MINODE *pathfollow;
    INODE *pip;
    OFT *new = (OFT*)(malloc(sizeof(OFT)));
    int val = 0, i = 0, hold = 1, empty = -1, check = 0;
    if(paths[0] == NULL)
    {
        paths[0] = readcustominput("Enter Pathname : ");
        strcpy(pathname, paths[0]);
        val = tokenize(paths[0]);
    }
    if(paths[1] == NULL)
    {
        paths[1] = readcustominput("Enter Mode to Open : ");
    }
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
    check = checktype(pathfollow);
    if(check == 0)
    {
        check = checkUmode(paths[1] , pathfollow);
        for(i = 0; i < 10; i++)
        {
            if(running->fd[i])
            {
                if(running->fd[i]->mptr->ino == pathfollow->ino && check != 0)
                {
                    printf("Error : file already opened\n");
                    return 0;
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
            return 0;
        }
        if(check)
        {
            if(check - 1 == 3)
            {
                new->offset = pathfollow->INODE.i_size;
            }
            else
            {
                new->offset = 0;
            }
            new->mode = check - 1;
            new->mptr = pathfollow;
            new->refCount++;
            running->fd[empty] = new;
        }
    }
    else
    {
        printf("File is of type directory and can't be opened\n");
        return 0;
    }
    return 0;
}