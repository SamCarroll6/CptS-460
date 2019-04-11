#include "header.h"

int link(void)
{
    MINODE *Path1, *parent;
    char *P0, *P1;
    int i = 0;
    if(name[0] && paths[1])
    {
        // Making copies because I want to avoid tokenizing 
        // originals as I need them intact.
        P0 = (char*)malloc(strlen(paths[0]) * sizeof(char));
        P1 = (char*)malloc(strlen(paths[1]) * sizeof(char));
        strcpy(P0, pathname);
        strcpy(P1, paths[1]);
        // Get existing file from passed in parameters
        if(!strcmp(name[0], "/"))
        {
            Path1 = findval(root);
        }
        else
        {
            Path1 = findval(running->cwd);
        }
        // If path1 doesn't exist return.
        if(!Path1)
        {
            printf("Error: Could not find file1\n");
            return -1;
        }
        // Ensure either regular or link file.
        INODE *check = &Path1->INODE;
        if(!S_ISREG(check->i_mode) && !S_ISLNK(check->i_mode))
        {
            printf("Error: file1 must be of type link or regular file.\n");
            return -1;
        }
        int ino = Path1->ino;
        // tokenize second passed in parameter. 
        tokenize(P1);
        // Get last value for name of actual new dir
        while(name[i])
        {
            i++;
        }
        i = i - 1;
        // Get bname of new file. 
        char bname[64];
        strcpy(bname, name[i]);
        // Set name[i] to NULL so it doesn't cause 
        // errors later when path is searched for location
        name[i] = NULL;
        // get MINODE of parent to new file
        if(!strcmp(bname, "/"))
        {
            printf("Error: Link cannot create file '/': File exists\n");
            return -1;
        }
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
        // Ensure parent is of type directory
        if(checktype(parent))
        {
            INODE *pip = &Path1->INODE;
            INODE *Parip = &parent->INODE;
            // Make sure parent doesn't have a file
            // with same name as bname
            if(search(Parip, bname) == -1)
            {
                // use mkdir enter_child funtction to 
                // create bname within parent.
                enter_child(parent, ino, bname);
                pip->i_links_count++;
                parent->dirty = 1;
                Path1->dirty = 1;
                iput(Path1);
                iput(parent);
                return 1;
            }
            printf("Error: cannot create link '%s': File exists\n", bname);
            return -1;
        }
    }
    printf("Usage: symlink file1 file2\n");
    return -1;
}