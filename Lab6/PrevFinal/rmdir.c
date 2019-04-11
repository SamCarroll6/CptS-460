#include "header.h"

/*
    Initial function for rmdir, goes through and ensures the directory
    can in fact be removed for various reasons. If it can it calls
    rm_child and provides parent minode and name of child to be removed.
*/
int rm_dir(void)
{
    int i = 0;
    MINODE *path, *parent;
    // Ensure pathname provided 
    if(name[0] == NULL)
    {    
        printf("Usage: rmdir requires pathname\n");
        return -1;
    }
    // Get MINODE of pathname
    if(!strcmp(name[0], "/"))
    {
        path = findval(root);
    }
    else
    {
        path = findval(running->cwd);
    }
    // Find bname of pathname
    while(name[i])
    {
        i++;
    }
    i = i - 1;
    // store bname
    char bname[64];
    strcpy(bname, name[i]);
    // replace bname with null in name[i] 
    // so findval still works
    name[i] = NULL;
    // checks to ensure directory can safely
    // be removed (such as not root, exists, and not cwd).
    if(path == root)
    {
        printf("rmdir: cannot remove '%s': cannot remove root\n", bname);
        return -1;
    }
    if(path == NULL)
    {
        printf("rmdir: cannot remove directory '%s': File does not exists\n", bname);
        return -1;
    }
    if(path == running->cwd)
    {
        printf("Error: cannot remove current working directory\n");
        return -1;
    }
    // Simply returns if bname equal to '.', based on actual linux
    // which won't delete dir when bname is '.'
    if(!strcmp(bname, "."))
    {
        printf("Error: failed to remove '.': Invalid argument\n");
        return -1;
    }
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
    // Ensure that path to be deleted is a directory
    // and is an empty directory.
    if(checktype(path) && is_empty(path))
    {
        // get inodes of parent and path
        INODE *pip = &parent->INODE, *cip = &path->INODE;
        // Deallocate blocks in dir to be removed. 
        for(i = 0; i < 12 && cip->i_block[i] != 0; i++)
        {
            bdalloc(path->dev, cip->i_block[i]);
        }
        // deallocate ino in dir to be removed.
        idalloc(path->dev, path->ino);
        // write deallocations back to disk
        iput(path);
        // run rm_child to remove bname from
        // parent directory
        int ret = rm_child(parent, bname);
        return ret;
    }
    // if the path wasn't directory or the directory wasn't empty 
    // print error
    printf("Path must be of type directory and must be empty\n");
    return -1;
}

/*
    Takes a parent minode and childs name in that minode,
    finds the child name in parent and removes it.
*/
int rm_child(MINODE *pmip, char *name)
{
    INODE *pip = &pmip->INODE;
    int size, i;
    char buf[BLKSIZE];
    char *cp;
    DIR *prev;
    size = pip->i_size;
    // Check all 12 direct blocks of parent for child.
    for(i = 0; i < 12; i++)
    {
        // once a block equals 0 we're done 
        // and child could not be found.
        if(pip->i_block[i] == 0)
        {
            printf("Error: Bname not found\n");
            return -1;
        }
        get_block(pmip->dev, pip->i_block[i], buf);
        dp = (DIR *)buf;
        cp = buf;
        char nameval[BLKSIZE + 1];
        // traverse given block pip->i_block[i] checking all entries 
        // for match to bname
        while(cp < &buf[BLKSIZE])
        {
            memcpy(nameval, dp->name, dp->name_len);
            nameval[dp->name_len] = '\0';
            // if we get a match of name within this block
	        if(!strcmp(nameval, name))
	        {
                // if it is first and only entry in a block
                // check if cp == buf (first entry in block)
                // and if cp + dp->rec_len = the end of the block.
                if(cp == buf && cp + dp->rec_len == &buf[BLKSIZE])
                {
                    // deallocate parents block i
                    bdalloc(pmip->dev, pip->i_block[i]);
                    // decrement parent size
                    pip->i_size -= BLKSIZE;
                    int count = i + 1;
                    // Traverse remaining blocks and move down
                    // one spot in i_block[i] to remove blanks.
                    for(count; count < 12; count++)
                    {
                        // Only matters if next block actually has content, otherwise
                        // they can all be 0.
                        if(pip->i_block[count])
                        {
                            // get next block and put at the previous blocks place
                            get_block(pmip->dev, pip->i_block[count], buf);
                            put_block(pmip->dev, pip->i_block[count-1], buf);
                        }
                        else
                        {
                            printf("Directory successfully removed\n");
                            break;
                        }
                    }
                }
                // if it is last entry in a block remove it and give previous 
                // reclen it's reclen so total is still BLKSIZE.
                else if(cp + dp->rec_len == &buf[BLKSIZE])
                {
                    prev->rec_len += dp->rec_len;
                    put_block(pmip->dev, pip->i_block[i], buf);
                }
                // dp is just an entry in the middle somewhere,
                // or start of a block with other records.
                else
                {
                    DIR *find_end = (DIR*)buf;
                    char *final_cp = buf;
                    // While loop to get the last entry in block.
                    while(final_cp + find_end->rec_len < &buf[BLKSIZE])
                    {
                        final_cp += find_end->rec_len;
                        find_end = (DIR*)final_cp;
                    }
                    find_end->rec_len += dp->rec_len;
                    // Move all records to the left to ensure
                    // data still fits.
                    char *moveS = cp + dp->rec_len;
                    char *moveE = buf + BLKSIZE;
                    // C function to move memory to the left.
                    memmove(cp, moveS, moveE-moveS);
                    put_block(pmip->dev, pip->i_block[i], buf);
                }
                pmip->dirty = 1;
                iput(pmip);
                return 1;
	        }
            prev = dp;
            cp += dp->rec_len;
            dp = (DIR*)cp;
        }
        pmip->dirty = 1;
        iput(pmip);
        return 1;
    }
    return -1;
}