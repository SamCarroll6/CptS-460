#include "header.h"


int myunlink(void)
{
    MINODE *mip, *parent;
    int i;
    // Ensure parameter was passed.
    if(name[0])
    {
        // Find MINODE of filename to be deleted
        if(!strcmp(name[0], "/"))
        {
            mip = findval(root);
        }
        else
        {
            mip = findval(running->cwd);
        }
        // Ensure filename is either type regular file or link
        INODE *pip = &mip->INODE;
        if(S_ISLNK(pip->i_mode) || S_ISREG(pip->i_mode))
        {
            for(i = 0; i < NFD; i++)
            {
                if(running->fd[i])
                {
                    if(running->fd[i]->mptr->ino == mip->ino)
                    {    
                        close_file(i);
                    }
                }
            }
            pip->i_links_count--;
            // if no other files linked to this ino we'll
            // deallocate the ino and blocks.
            if(pip->i_links_count <= 0)
            {
                for(i = 0; i < 12 && pip->i_block[i] != 0; i++)
                {
                    bdalloc(mip->dev, pip->i_block[i]);
                }
                 idalloc(mip->dev, mip->ino);
            }
            // Get bname of file to be deleted.
            i = 0;
            while(name[i])
            {
                i++;
            }
            i = i - 1;
            char bname[64];
            strcpy(bname, name[i]);
            name[i] = NULL;
            // Get MINODE of parent to path
            if(name[0] == NULL)
            {
                parent = findval(running->cwd);
            }
            else if(!strcmp(name[0], "/"))
            {
                parent = findval(root);
            }
            else
            {
                parent = findval(running->cwd);
            }
            // Run rm_child from rm_dir on parent and 
            // to be deleted bname
            rm_child(parent, bname);
            return 1;
        }
        printf("Error: Pathname must be of type link or regular file\n");
    }
    printf("Usage: unlink requires pathname\n");
    return -1;
}