#include "header.h"

int mysymlink(void)
{
    MINODE *Path1, *Path2;
    char *P0, *P1;
    if(name[0] && paths[1])
    {
        // Making copies because I want to avoid tokenizing 
        // originals as I need it intact.
        P0 = (char*)malloc(strlen(paths[0]) * sizeof(char));
        P1 = (char*)malloc(strlen(paths[1]) * sizeof(char));
        strcpy(P0, pathname);
        strcpy(P1, paths[1]);
        // Find path of existing value. 
        if(!strcmp(name[0], "/"))
        {
            Path1 = findval(root);
        }
        else
        {
            Path1 = findval(running->cwd);
        }
        if(!Path1)
        {
            printf("Error: Could not find file1\n");
            return -1;
        }
        // Ensure existing path is type regular or type directory
        INODE *check = &Path1->INODE;
        if(!S_ISREG(check->i_mode) && !S_ISDIR(check->i_mode))
        {
            printf("Error: file1 must be of type directory or regular file.\n");
            return -1;
        }
        // Filename length can't be longer than 60.
        // according to KC. 
        if(strlen(P0) >= 60)
        {
            printf("Error: file1 name too large symlnk failed\n");
            return -1;
        }
        // change name[] from file1 to file2
        // tokenize(paths[1]);
        tokenize(P1);
        if(creat_file() == -1)
        {
            printf("Error: symlink failed\n");
            return -1;
        }
        // creat_file overwrites name[] so we'll
        // recreate it again.
        strcpy(P1, paths[1]);
        tokenize(P1);
        // Find MINODE of newly created file representing 
        // the file to be linked.
        if(!strcmp(name[0], "/"))
        {
            Path2 = findval(root);
        }
        else
        {
            Path2 = findval(running->cwd);
        }
        if(!Path2)
        {
            printf("Error: Could not find file2\n");
            return -1;
        }
        // Ensure new file has proper mode for link
        INODE *pip = &Path2->INODE;
        pip->i_mode = 0120777;
        // if the strlen of the path is less than 60
        // store the name because it fits 
        // according to KC you have room for 60.
        strcpy(pip->i_block, P0);
        pip->i_size = strlen(P0);
        Path2->dirty = 1;
        iput(Path2);
        // if not just store the size for read link to
        // use.
        free(P0);
        free(P1);
        return 1;
    }
    printf("Usage: symlink file1 file2\n");
    return -1;
}

char* myreadlink(void)
{
    MINODE *mip; 
    if(name[0] == NULL)
    {
        printf("Usage: readlink file1\n");
        return -1;
    }
    if(!strcmp(name[0], "/"))
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
        if(S_ISLNK(pip->i_mode))
        {
            char buf[64];
            strcpy(buf, (char*)pip->i_block);
            return buf;
        }
        printf("Error: file1 must be type link\n");
    }
    return -1;
}