#include "header.h"

int touch(void)
{
    MINODE *Path1;
    if(name[0])
    {
        // Find and adjust time.
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
            printf("Error: could not find file %s: touch failed\n", pathname);
            return -1;
        }
        INODE *pip = &Path1->INODE;
        pip->i_ctime = time(NULL);
        Path1->dirty = 1;
        iput(Path1);
        return 1;
    }
    printf("Usage: touch requires pathname\n");
    return -1;
}